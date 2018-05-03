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
int total_benchmark = 0;
//The function for each thread to run
void *connection_handler(void*);

int done = 0;

int line_counter = 0;

struct thread_send {
  int sock;
  int thread_id;
  int benchmark;
};

int main()
{
  int socket_desc, client_sock, c, i;
  struct sockaddr_in server, client;
  int connections = 0;
  pthread_t thread_id[10];
  char answer[10];
  int size, benchmark;
  char incoming_message[4096];
  int message_type;
  char client_ip[20];

  struct thread_send all_args[10];
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


    all_args[connections].sock = client_sock;
    all_args[connections].thread_id =  connections;


    memset(incoming_message,0,4096);
    size = recv(client_sock,incoming_message, 4096, 0);
    //previous_performance = benchmark

    sscanf(incoming_message,"%d %s %d",&message_type,client_ip,&benchmark);
    total_benchmark += benchmark;
    all_args[connections].benchmark = benchmark;

    connections ++;
    printf("Any more connections to make (Y/N)?\n");
    fgets(answer,10,stdin);

    if(strstr(answer,"N") != 0)
    {
      break;
    }

  }

  for(i = 0; i < connections; i++)
  {
    if(pthread_create(&thread_id[i], NULL, connection_handler, (void*) &all_args[i]) < 0)
    {
      printf("Could not create threads\n");
    }
  }

  for(i = 0; i < connections; i++)
  {
    pthread_join(thread_id[i],NULL);
  }
  pthread_mutex_destroy(&lock);

  return 0;
}

void *connection_handler(void*a)
{
  int size, lines_to_send;
  struct thread_send* args = (struct thread_send*)a;
  int sock = args->sock;
  char incoming_message[4096];
  char send_buffer[2048];
  char result[1024];
  char client_ip[40];
  int id = args->thread_id;
  int benchmark = args->benchmark;//This needs to be a local variable
  int message_type;
  char * password;
  lines_to_send = (int)(((float)benchmark/(float)total_benchmark) * 10000);
  printf("client (%d), benchmark: %d out of %d\n, lines to send %d, line_counter %d\n",id,benchmark, total_benchmark,lines_to_send, line_counter);

  //while the line counter is less than the number of lines or done == false
  while(line_counter < LINES_IN_FILE && done == 0)
  {
    //select the number of lines to send (based on previous performance)
    //benchmark * 2000;
    //send the client the line counter and the number of lines
    sprintf(send_buffer,"%s %d %d",hashed_password,line_counter,line_counter + lines_to_send);

    printf("In the client (%d) sending %d,%d\n",id,line_counter,line_counter+lines_to_send);
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
    printf("getting a reply\n");
    memset(incoming_message,0,4096);
    size = recv(sock,incoming_message,4096,0);

    incoming_message[size] = "\0";
    sscanf(incoming_message,"%d %d %s",&message_type,&benchmark,result);

    if((password = strstr(result,":")) != 0)
    {
      printf("Password found %s\n",++password);
      done = 1;
      if(send(sock,"exit\0",5,0) == -1)
      {
        printf("Can't tell client to exit");
        return 1;
      }
      return 0;
    }

  }


  if(send(sock,"exit\0",5,0) == -1)
  {
    printf("Can't tell client to exit");
    return 1;
  }
 //If we ever found the answer, print it out

  return 0;
}
