#include "utils.hpp"

Server::Server (int socketServer) : _nb_client(1), _nb_client_channel(0)
{
    this->fds[0].fd = socketServer;
	this->fds[0].events = POLLIN;
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