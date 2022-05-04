#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <iostream>

typedef struct User{
	char nom[30];
	int age;
}User;

void *function(void *arg)
{
	int socketClient = *(int*)arg;
	std::string temp;
	getline(std::cin, temp);
	int len = 500;
	char msg[len];
	int i = 0;
	while(temp[i])
	{
		msg[i] = temp[i];
		i++;
	}
	msg[i] = '\0';
	send(socketClient, msg, len, 0);
	char msg_receive[len];
	recv(socketClient, msg_receive, len, 0);
	std::cout << msg_receive << std::endl;
	close(socketClient);
	delete((int*)arg);
	pthread_exit(NULL);
}

int main(void)
{
	int socketServer = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addrServer;
	addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(30000);

	bind(socketServer, (const struct sockaddr *)&addrServer, sizeof(addrServer));
	std::cout << "bind: " << socketServer << std::endl;

	listen(socketServer, 5);
	std::cout << "listen" << std::endl;

	pthread_t threads[3];

	for(int i = 0; i < 3; i++){
		struct sockaddr_in addrClient;
		socklen_t csize = sizeof(addrClient);
		int socketClient = accept(socketServer, (struct sockaddr *)&addrClient, &csize);
		std::cout << "accept" << std::endl;

		std::cout << "client: " << socketClient << std::endl;
		
		int *arg = new int();
		*arg = socketClient;
		pthread_create(&threads[i], NULL, function, arg);
	}

	for(int i = 0; i < 3; i++){
		pthread_join(threads[i], NULL);
	}

	//send(socketClient, &user, sizeof(user), 0);

	close(socketServer);
	std::cout << "close" << std::endl;
	return 0;
}