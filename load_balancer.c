//load_balancer.c - this file holds the main program for the load load_balancer
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <string.h>
#define PORT 34768
//#define LINES_IN_FILE 76094863
#define LINES_IN_FILE 129988
pthread_mutex_t lock;
char hashed_password[1024];
//The function for each thread to run
void *connection_handler(void*);

int main()
{
  int socket_desc, client_sock, c, i;
  struct sockaddr_in server, client;
  int connections = 0;
  pthread_t thread_id[10];

  //Steps
  //Get the password from the user as an input
  printf("Please input the hashed password: ");
  fgets(hashed_password,1024,stdin);

  socket_desc = socket(AF_INET,SOCK_STREAM,0);
  if(socket_desc == -1)
  {
    printf("ERROR: could not create socket\n");
    return 1;
  }
  printf("Socket created\n");

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(PORT);

  if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
    printf("ERROR: failed to bind\n");
    return 1;
  }
  printf("Yay it hath bound. \nWaiting for connections...\n");

  listen(socket_desc,3);

  c = sizeof(struct sockaddr_in);

  if(pthread_mutex_init(&lock, NULL) != 0)
  {
    printf("Mutex init failed\n");
    return 1;
  }

  while(client_sock=accept(socket_desc,(struct sockaddr*)&client,(socklen_t*)&c))
  {
    printf("Accepted a connection\n");
    if(pthread_create(&thread_id[connections], NULL, connection_handler, (void*) &client_sock) < 0)
    {
      printf("Could not create thread\n");
    }
    connections ++;
  }

  for(i = 0; i <= connections; i++)
  {
    pthread_join(thread_id[i],NULL);
  }
  pthread_mutex_destroy(&lock);

  return 0;
}

void *connection_handler(void*socket_desc)
{
  //This holds the current line they are on
  int line_counter = 0;
  int done = 0;
  int size, lines_to_send;
  int sock = *(int*)socket_desc;
  char incoming_message[4096];
  char send_buffer[2048];
  char result[1024];
  char client_ip[40];
  int benchmark;
  int message_type;

  //get the first message with the benchmarks from the client
  size = recv(sock,incoming_message, 4096, 0);
  //previous_performance = benchmark

  sscanf(incoming_message,"%d %s %d",&message_type,client_ip,&benchmark);
  printf("Now we have benchmark: %d and ip %s\n",benchmark,client_ip);
  //while the line counter is less than the number of lines or done == false
  while(line_counter < LINES_IN_FILE && done == 0)
  {
    //select the number of lines to send (based on previous performance)
    lines_to_send = 200;//benchmark * 2000;
    //send the client the line counter and the number of lines
    sprintf(send_buffer,"%s %d %d",hashed_password,line_counter,line_counter + lines_to_send);
    if(send(sock,send_buffer,strlen(send_buffer),0) == -1)
    {
      printf("Unable to send to client");
      return 1;
    }
    //lock the line counter and update it + num lines
    pthread_mutex_lock(&lock);
    line_counter += lines_to_send;
    pthread_mutex_unlock(&lock);

    //spin around waiting for a reply from the client saying they are done or
    //have found the item
    size = recv(sock,incoming_message,4096,0);
    incoming_message[size] = "\0";
    sscanf(incoming_message,"%d %d %s",&message_type,&benchmark,result);
    printf("Got %s",incoming_message);

    return 1;


    //Take the time they took to do it and compare with expected to get
    //previous_performance
  }
 //If we ever found the answer, print it out

  return 0;
}
