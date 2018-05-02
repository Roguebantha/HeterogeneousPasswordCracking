all: client server
client: Client.c
	gcc Client.c -o client
server: load_balancer.c
	gcc load_balancer.c -o load_balancer -lpthread
debug: Client.c
	gcc Client.c -o client -g
