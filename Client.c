#include<connector.h>
#include<time.h>
#define PORT 34768
unsigned int benchmark() {
	return 1;
}

void crack(unsigned int start, unsigned int end, char* result,unsigned int result_length) {
//TODO
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
		printf("Waiting for job..."
		char reply[512];
		if(get_reply(reply,512) < 0) {
			printf("FATAL: Did not recieve response from server\n");
			return 2;
		}
		//TODO parse reply
		char result[4096];
		char result_reply[4096];
		time_t t0 = time(NULL);
		crack(start,end,result,4096);
		sprintf(result_reply,"1 %u %s",time(NULL) - t0,result);
		send_bytes(result_reply,strlen(result_reply));
	}
}
