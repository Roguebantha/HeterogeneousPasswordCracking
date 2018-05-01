/* As writer of this code, I hereby put this code in the public domain.
 * Feel free to use, edit, release, and republish as the user deems.
 * Credit is appreciated but is by no means required
 * Roguebantha
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
static int sock = -1;
int send_bytes(char *data,unsigned int length) {
	return send(sock,data,length,0);
}
int get_reply(char* reply,unsigned int reply_len) {
	memset(reply,0,reply_len);
	return recv(sock, reply, reply_len,0);
}
int connect_to_server(short port, char* ip) {
    static struct sockaddr_in server;
    if(sock == -1) {
        //Create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
            return 1;
        server.sin_addr.s_addr = inet_addr(ip);
        server.sin_family = AF_INET;
        server.sin_port = htons( port );
        //Connect to remote server
        if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
            return 2;
        return 0;
    }
    return 3;
}
int close_connection() {
	if(sock < 0)
		return 1;
	return close(sock);
}
