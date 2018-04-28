#include "connector.h";
#include<time.h>

#define PORT 34768
const char filename[128] = "all_passwords.dict";
unsigned int benchmark() {
	return 1;
}

void crack(char* hash, unsigned int start, unsigned int end, char* result,unsigned int result_length) {
	char command[256];
	FILE *hashfile =  fopen("hashfile.hash","rw");
	fputs(hash,hashfile);
	fclose(hashfile);
	sprintf(command,"tail -n %u %s | head -n $((%u-%u+1)) > passwords.dict",start,filename,end,start);
	system(command);
	system("cat passwords.dict | hashcat64.bin hashfile.hash > output 2> output");
}
int main() {
	char server_ip[16];
	printf("Enter server ip: ");
	fgets(server_ip,16,stdin);
	char client_ip[16];
	printf("Enter client ip: ");
	fgets(client_ip,16,stdin);
	if(connect_to_server(PORT,client_ip)) {
		printf("FATAL: Failed to connect to server\n");
		return 1;
	}
	char init_connect[64];
	sprintf(init_connect,"0 %s %u",client_ip,benchmark());
	if(send_bytes(init_connect,strlen(init_connect)) < 0) {
		printf("FATAL: Failed to send data to server\n");
		return 2;
	}
	while(1) {
		printf("Waiting for job...");
		char reply[512];
		if(get_reply(reply,512) < 0) {
			printf("FATAL: Did not recieve response from server\n");
			return 2;
		}
		//TODO parse reply
		time_t t0 = time(NULL);
		crack(start,end,result,4096);

		FILE *output = fopen("output", "rb");
		fseek(output, 0, SEEK_END);
		long output_size = ftell(output);
		fseek(output, 0, SEEK_SET);  //same as rewind(f);
		char *result = malloc(output_size + 1);
		char *result_reply = malloc(output_size+30);
		fread(result, output_size, 1, f);
		fclose(output);
		result[output_size] = 0;

		sprintf(result_reply,"1 %u %s",time(NULL) - t0,result);
		send_bytes(result_reply,strlen(result_reply));
	}
}
