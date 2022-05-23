#include "server.hpp"

Server::Server(void) : _clients(0) 
{
    this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    this->_addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    this->_addrServer.sin_family = AF_INET;
    this->_addrServer.sin_port = htons(31933);
    struct pollfd lserver;

	bind(this->_serverSocket, (const struct sockaddr *)&this->_addrServer, sizeof(this->_addrServer));
    std::cout << "bind : " << this->_serverSocket << std::endl;
    listen(this->_serverSocket, 5);
    std::cout << "listening at : " << ntohs(this->_addrServer.sin_port) << std::endl;
	lserver.fd = this->_serverSocket;
	lserver.events = POLLIN;
    this->_lfds.push_back(lserver);
    build_fds();
	return ;
}

Server::~Server(void) {
	return ;
}


void Server::addUser() 
{
	if(this->_clients == 9)
		return ;
	clients new_cli;
    struct pollfd new_fd;

    new_cli.csize = sizeof(new_cli.addrClient);
    new_cli.socket = accept(this->_serverSocket, (struct sockaddr *)&new_cli.addrClient, &new_cli.csize);
    new_cli.nb_msg = 0;
    std::cout << "USER[" << new_cli.socket << "]->[" << inet_ntoa(new_cli.addrClient.sin_addr) <<"] connected." << std::endl;
	new_fd.fd = new_cli.socket;
	new_fd.events = POLLIN;
    send(new_fd.fd, this->_wlcmsg.data(), this->_wlcmsg.size(), 0);
    send(new_fd.fd, this->_wlcmsg2.data(), this->_wlcmsg2.size(), 0);
	this->_clients++;
    this->_lfds.push_back(new_fd);
    this->_user_data.push_back(new_cli);
    build_fds();
	return ;
}

bool Server::channel_open(std::string channel_name)
{
    for (std::list<channel>::iterator beg = this->_channel_data.begin(); beg != this->_channel_data.end(); beg++)
    {
        if (beg->name == channel_name)
        {    
            beg->nb_client++;
            return true;
        }    
    }
    return false;
}

void Server::channel_empty(std::string channel_name)
{
    std::list<channel>::iterator it = this->_channel_data.begin();
    std::list<channel>::iterator ite = this->_channel_data.end();
    while ( it->name != channel_name && it != ite)
        it++;
    if (it != ite){
        it->nb_client--;
        if (it->nb_client == 0){
            std::cout << "channel " <<  channel_name << " is close \n";
            this->_channel_data.erase(it);
        }
    }
    return ;
}

void Server::create_channel(int user, std::list<clients>::iterator it_cli, std::string msg)
{
    it_cli->channel.insert(it_cli->channel.begin(), msg.begin()+5, msg.end());
    std::cout << "channel " << it_cli->channel << " creer\n";
    if (this->_channel_data.size() == 0){
        channel channel;
        channel.name = it_cli->channel;
        channel.nb_client = 1;
        this->_channel_data.push_back(channel);
    }
    else {
        if (channel_open(it_cli->channel) == false){
            channel channel;
            channel.name = it_cli->channel;
            channel.nb_client = 1;
            this->_channel_data.push_back(channel);
        }  

    }
    send(it_cli->socket, "channel creer ", 14, 0);
    it_cli->nb_msg++;
}

void Server::user_left(std::list<pollfd>::iterator it)
{
    std::list<clients>::iterator it_cli = this->_user_data.begin();   
    while (it_cli->socket != it->fd)
        it_cli++;
    std::cout << "USER[" << it->fd << "] disconnected." << std::endl;
    close(it->fd);
    this->_clients--;
    std::list<pollfd>::iterator beg = this->_lfds.begin();
    while (beg->fd != it->fd)
        beg++;
    this->_lfds.erase(beg);
    channel_empty(it_cli->channel);
    this->_user_data.erase(it_cli);
    build_fds();
    return ;
}

void Server::setup_username( std::string username, std::list<clients>::iterator it_cli, int first )
{
    //std::cout << "test = " << "|"<< username << "|"  << "   first " << first << std::endl;
    //std::cout << "test = " << username.size() << std::endl;
    /* if (username.size() > 14)
    {
        send(it_cli->socket, "Username is under 9 character.", 30, 0);
        return ;
    } */
    if (it_cli->username.empty() == false)
        it_cli->username.clear();
    it_cli->username.insert(it_cli->username.begin(), username.begin()+first+5, username.end());
    return ;
}

void Server::setup_password( std::string password, std::list<clients>::iterator it_cli )
{
    if (it_cli->password.empty() == false)
        it_cli->password.clear();
    it_cli->password.insert(it_cli->password.begin(), password.begin()+6, password.end());
    return ;
}

void Server::delete_clrf(std::string temp)
{
    int i = 0;
    int to_fill = 0;
    char wait[temp.size()];

    while (i > temp.size())
    {
        if (temp[i] != '\n' && temp[i] != '\r')
        {
            wait[to_fill] = temp[i];
            to_fill++;
        }
        i++;
    }
    temp.clear();
    temp.assign(wait);
    return ;
}

void Server::servListen(std::list<pollfd>::iterator it) 
{
	User user;
    std::string temp;
    std::string nick = "NICK ";
    int rec;
    //std::cout << "voici le fd " << it->fd << std::endl;
    if(it->revents & POLLIN){
        rec = recv(it->fd, &user.msg, sizeof(user.msg), 0);
        temp.assign(user.msg);
        std::cout << "msg before = " << temp << std::endl;
        delete_clrf(temp);
        std::cout << "msg = " << temp << std::endl;
        if(rec == 0 || temp == "quit")
            user_left(it);
		else 
        {
            std::list<clients>::iterator it_cli = this->_user_data.begin();  
            while (it_cli->socket != it->fd)
                it_cli++;
            if (temp.find("JOIN ", 0, 5) != std::string::npos)
                create_channel(it->fd, it_cli, temp);
            if (temp.find(nick, 0) != std::string::npos){
                setup_username(temp, it_cli, temp.find(nick, 0));
            }
            /* if (temp.find("/PASS ", 0, 6) != std::string::npos)
                setup_password(temp, it_cli); */
            else
            {
                if (it_cli->channel.empty() == false)
       	        {
                    temp = temp + "\r\n";
                    std::list<clients>::iterator to_send = this->_user_data.begin(); 
                    for(std::list<clients>::iterator to_send = this->_user_data.begin(); to_send != this->_user_data.end(); to_send++)
                    {
                        if (it_cli->socket != to_send->socket)
                            send(to_send->socket, temp.data() , temp.size(), 0);
                    }
                }
            }
            
        }
    }
return ;
}

struct pollfd *Server::get_fds( void )
{
	return this->_fds;
}

std::list<pollfd> Server::get_lfds( void )
{
    return this->_lfds;
}

void Server::build_fds(void) 
{
    int i = 0;
    for(std::list<pollfd>::iterator it = this->_lfds.begin(); it != this->_lfds.end(); it++)
    {
        this->_fds[i] = *it;
        i++;
    }
    return ;
}

void Server::update_revents( void ) 
{
    std::list<pollfd>::iterator it = this->_lfds.begin();
    for(int i = 0; i < this->_lfds.size(); i++) 
    {
        it->revents = this->_fds[i].revents;
        it->events = this->_fds[i].events;
        it++;
    }
    return ;
}

void Server::display_fds( void )
{
    std::list<channel>::iterator it = this->_channel_data.begin();
    std::list<channel>::iterator ite = this->_channel_data.end();

    while(it != ite)
    {
        it++;
    }
    return ;
}