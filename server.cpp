#include "utils.hpp"

Server::Server (void ) : _nb_client(1), _nb_client_channel(0)
{
    int socketServer = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrServer;

    addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(30011);

    bind(socketServer, (const struct sockaddr *)&addrServer, sizeof(addrServer));
    std::cout << "bind ; " << socketServer << std::endl;

    listen(socketServer, 5);
    this->_fds[0].fd = socketServer;
	this->_fds[0].events = POLLIN;
    std::cout << "listen" << std::endl;
    return ;
}

Server::~Server (void)
{
    return ;
}

int Server::get_nb_client( void )
{
    return this->_nb_client;
}

int Server::get_nb_client_channel( void )
{
    return this->_nb_client_channel;
}

struct pollfd* Server::get_fds( void )
{
    return this->_fds;
}

void Server::add_client_channel (void )
{
    User user;
    _Server to_push;
    to_push.csize  = sizeof(to_push.addrClient);
    to_push.socketClient = accept(_fds[0].fd, (struct sockaddr *)&to_push.addrClient, &to_push.csize);
    std::cout << "accept" << std::endl;
    _fds[_nb_client].fd = to_push.socketClient;
    _fds[_nb_client].events = POLLIN;
    send(_fds[_nb_client].fd, "Welcome, What is your nickname ?", 32, 0);
    to_push.nb_msg = 0;
    this->_nb_client++;
    this->_nb_client_channel++;
    this->inf_client.push_back(to_push);
    return ;
}

void Server::send_msg ( int x )
{
    User user;
    if(this->_fds[x].revents & POLLIN)
    {
        if(recv(this->_fds[x].fd, &user, sizeof(User), 0) == 0)
        {
            std::cout << this->inf_client[x - 1].nickname << " disconnected." << std::endl;
            close(this->_fds[x].fd);
            this->_nb_client_channel--;
            this->inf_client.erase(this->inf_client.begin() + x - 1);
            if (this->_nb_client_channel == 0)
            {
                std::cout << "channel close \n";
                exit(0);
            }
		}
		else
        {
            if (this->inf_client[x - 1].nb_msg == 0)
            {
                std::string test(user.msg);
                if ( test.size() > 8)
                {
                    send(this->_fds[x].fd, "Sorry nickname is under 8 character try again.", 32, 0);    
                }
                else 
                {
                    send(this->_fds[x].fd, "Valide", 10, 0); 
                    this->inf_client[x - 1].nickname = test;
                    this->inf_client[x - 1].nb_msg++;
                }
            }
            else 
                std::cout << this->inf_client[x - 1].nickname << " : " << user.msg << std::endl;
        }
    }
    return ;
}