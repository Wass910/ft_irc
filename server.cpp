#include "utils.hpp"

Server::Server (void ) : _nb_client(1), _nb_client_channel(0)
{
    int socketServer = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrServer;

    addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(30023);

    bind(socketServer, (const struct sockaddr *)&addrServer, sizeof(addrServer));
    std::cout << "bind ; " << socketServer << std::endl;

    listen(socketServer, 5);
    this->fds[0].fd = socketServer;
	this->fds[0].events = POLLIN;
    std::cout << "listen" << std::endl;
    return ;
}

Server::~Server (void)
{
    return ;
}

void Server::add_client_channel (void )
{
    _Server to_push;
    to_push.csize  = sizeof(to_push.addrClient);
    to_push.socketClient = accept(fds[0].fd, (struct sockaddr *)&to_push.addrClient, &to_push.csize);
    std::cout << "accept" << std::endl;
    fds[_nb_client].fd = to_push.socketClient;
    fds[_nb_client].events = POLLIN;
    send(fds[_nb_client].fd, "welcome ;)", 11, 0);
    this->_nb_client++;
    this->_nb_client_channel++;
    this->inf_client.push_back(to_push);
    return ;
}