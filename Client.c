#include "connector.h"
#include<time.h>

#define PORT 34768
const char filename[128] = "all_passwords.dict";
const char hash_filename[128] = "hashfile.hash";
const unsigned int hash_type = 400;
unsigned int benchmark() {
	system("cd ./hashcat-4.1.0/ && cat example.dict | ./hashcat64.bin -m400  example400.hash --speed-only --machine-readable --quiet -O | grep 1: | cut -d : -f2 > ../benchmark_score");
	unsigned int score;
	FILE *benchmark = fopen("benchmark_score", "rb");
	fscanf(benchmark,"%u",&score);
	fclose(benchmark);
	return score;
}

void crack(char* hash, unsigned int start, unsigned int end) {
	char command[256];
	FILE *hashfile =  fopen(hash_filename,"ab+");
	fputs(hash,hashfile);
	fputs("\n",hashfile);
	fclose(hashfile);
	sprintf(command,"tail -n +%u %s | head -n $((%u-%u+1)) > passwords.dict",start,filename,end,start);
	printf("%s\n",command);
	system(command);
	sprintf(command,"cat passwords.dict | ./hashcat-4.1.0/hashcat64.bin -m %d --machine-readable --quiet -O %s > output 2> output",hash_type,hash_filename);
	printf("%s\n",command);
	system(command);
}
int main() {
	char server_ip[16];
	char client_ip[16];
	char hash[64] = "$H$9y5boZ2wsUlgl2tI6b5PrRoADzYfXD1";
	printf("Enter server ip: ");
	fgets(server_ip,16,stdin);
	printf("Enter client ip: ");
	fgets(client_ip,16,stdin);
	*strstr(client_ip,"\n") = 0;
	printf("Benchmarking...\n");
	unsigned int score = benchmark();
	printf("Connecting...\n");
	if(connect_to_server(PORT,server_ip)) {
		printf("FATAL: Failed to connect to server\n");
		return 1;
	}
	char init_connect[64];
	sprintf(init_connect,"0 %s %u",client_ip,score);
	if(send_bytes(init_connect,strlen(init_connect)) < 0) {
		printf("FATAL: Failed to send data to server\n");
		return 2;
	}
	while(1) {
		printf("Waiting for job...\n");
		char reply[512];
		if(get_reply(reply,512) < 0) {
			printf("FATAL: Did not recieve response from server\n");
			return 2;
		}
		if(strstr(reply,"exit")) return 0;
		int start = 0,end = 129987;
		//TODO parse reply
		time_t t0 = time(NULL);
		crack(hash,start,end);

		FILE *output = fopen("output", "rb");
		fseek(output, 0, SEEK_END);
		long output_size = ftell(output);
		fseek(output, 0, SEEK_SET);
		char *result = malloc(output_size + 1);
		char *result_reply = malloc(output_size+30);
		fread(result, output_size, 1, output);
		fclose(output);
		result[output_size] = 0;

		sprintf(result_reply,"1 %u %s",(unsigned int)(time(NULL) - t0),result);
		send_bytes(result_reply,strlen(result_reply));
	}
}
