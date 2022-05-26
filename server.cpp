#include "server.hpp"

Server::Server(void) {
	return ;
}

Server::Server( int port ) : _clients(0) 
{
    this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    this->_addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    this->_addrServer.sin_family = AF_INET;
    this->_addrServer.sin_port = htons(port);
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
	this->_clients++;
    this->_lfds.push_back(new_fd);
    this->_user_data.push_back(new_cli);
    build_fds();
	return ;
}

bool Server::channel_open(std::string channel_name, int user)
{
    for (std::list<channel>::iterator beg = this->_channel_data.begin(); beg != this->_channel_data.end(); beg++)
    {
        if (beg->name == channel_name)
        {    
            beg->nb_client++;
            beg->client_socket.push_back(user);
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

void Server::create_channel(int user, std::list<clients>::iterator it_cli, std::string msg, std::string channel_name)
{
    it_cli->channel.push_back(channel_name);
    if (this->_channel_data.size() == 0){
        std::cout << "channel " << channel_name << " creer\n";
        channel channel;
        channel.name = channel_name;
        channel.nb_client = 1;
        channel.client_socket.push_back(user);
        this->_channel_data.push_back(channel);
    }
    else {
        if (channel_open(channel_name, user) == false){
            std::cout << "channel " << channel_name << " existe\n";
            channel channel;
            channel.name = channel_name;
            channel.nb_client = 1;
            channel.client_socket.push_back(user);
            this->_channel_data.push_back(channel);
        }  
    }
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
    //channel_empty(it_cli->channel);
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
    for(std::list<clients>::iterator to_send = this->_user_data.begin(); to_send != this->_user_data.end(); to_send++)
    {
        if (username == to_send->username)
        {
            username = username + "_";
            break ;
        }
    }
    if (it_cli->username.empty() == false)
        it_cli->username.clear();
    it_cli->username.insert(it_cli->username.begin(), username.begin()+first+6, username.end());
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
    std::string first;
    std::string::iterator it = temp.begin();
    std::string::iterator position = temp.begin();

    while (it != temp.end())
    {
        if (*it == '\r')
        {
            first.assign(position, it);
            this->cmd.push_back(first);
            first.clear();
            position = it;
            position = position + 2;
        }
        it++;
    }
    for (std::vector<std::string>::iterator it = this->cmd.begin(); it != this->cmd.end(); it++)
    {
        std::cout << "cmd = |" << *it << "|\n";
    }

    return ;
}

void Server::setup_host( std::string host, std::list<clients>::iterator it_cli )
{
    int zero = 0;
    std::string::iterator it = host.begin();
    std::string::iterator position = host.begin();
    
    while (it != host.end())
    {
        if (*it == ' ')
        {    
            zero++;
            position = it;
            position++;
        }
        it++;
        if (zero == 2)
            break;
    }
    while (it != host.end())
    {
        if (*it == ' ')
            break;
        it++;
    }
    if (it_cli->host.empty() == false)
        it_cli->host.clear();
    it_cli->host.assign(position, it);
    return ;
}

std::string Server::cut_word_space( std::string to_cut, std::string::iterator it )
{
    std::string after_cut;
    std::string::iterator it_space = it;
    while (*it_space != ' ')
        it_space++;
    
    after_cut.assign(it, it_space);
    std::cout << "after = |" << after_cut << "|\n";
    return after_cut;
}

void Server::commandPART(std::list<clients>::iterator it_cli, std::vector<std::string>::iterator it)
{
    int position = -1;
    std::list<int>::iterator to_del;
    if (it->find('#') != std::string::npos)
        position = it->find('#');
    std::string channel_name;
    if (position != -1)
        channel_name.assign(it->begin() + position, it->end());
    std::cout << "channel = |" << channel_name << "|\n";
    *it = *it + "\r\n";
    *it = ":" + it_cli->username + "!" + it_cli->host + "@" + it_cli->host + " " + *it;
    for(std::list<channel>::iterator to_send = this->_channel_data.begin(); to_send != this->_channel_data.end(); to_send++)
    {
        if (channel_name == to_send->name)
        {

            for (std::list<int>::iterator socket_in_channel = to_send->client_socket.begin(); socket_in_channel != to_send->client_socket.end(); socket_in_channel++){
                std::cout << "socket in the channel " << *socket_in_channel << std::endl;
            }
            std::cout << "priv msg channel = |" << *it << "|\n";
            for (std::list<int>::iterator socket_in_channel = to_send->client_socket.begin(); socket_in_channel != to_send->client_socket.end(); socket_in_channel++){
                if (it_cli->socket == *socket_in_channel ){
                    to_del = socket_in_channel;
                    std::cout << "a supp " << *to_del << std::endl;}
                else
                    send(*socket_in_channel, it->c_str() , it->size(), 0);
            }
            to_send->client_socket.erase(to_del);
        }
        }
    it_cli->channel.clear();
    return ;
}

void Server::commandJOIN( std::list<clients>::iterator it_cli, std::vector<std::string>::iterator it )
{
    int position = -1;
    std::string channel_name;
    if (it->find('#') != std::string::npos)
        position = it->find('#');
    channel_name.assign(it->begin() + position, it->end());
    create_channel(it_cli->socket, it_cli, *it, channel_name);
    *it = *it + "\r\n";
    std::list<clients>::iterator to_send = this->_user_data.begin(); 
    *it = ":" + it_cli->username + "!" + it_cli->host + "@" + it_cli->host + " " + *it;
    for(std::list<channel>::iterator to_send = this->_channel_data.begin(); to_send != this->_channel_data.end(); to_send++)
    {
        if (channel_name == to_send->name)
        {
            std::cout << "priv msg channel = |" << *it << "|\n";
            for (std::list<int>::iterator socket_in_channel = to_send->client_socket.begin(); socket_in_channel != to_send->client_socket.end(); socket_in_channel++){
                if (it_cli->socket != *socket_in_channel)
                    send(*socket_in_channel, it->c_str() , it->size(), 0);
            }
        }
    }
}

void Server::commandNICK( std::list<clients>::iterator it_cli, std::vector<std::string>::iterator it )
{
    std::string old_username = it_cli->username;
    setup_username(*it, it_cli, it->find("NICK", 0, 5));
    std::string output = ":" + old_username + " NICK " + it_cli->username + "\r\n";
    send(it_cli->socket, output.c_str() , output.size(), 0);
}

void Server::commandPRIVMSG( std::list<clients>::iterator it_cli, std::vector<std::string>::iterator it )
{
    int position = -1;
    std::string channel_name;
    std::string user_to_send;
    if (it->find('#') != std::string::npos)
        position = it->find('#');
    *it = *it + "\r\n";
    if (position != -1)
        channel_name = cut_word_space(*it, it->begin() + position);
    if (channel_name.empty() == false){
    *it = ":" + it_cli->username + "!" + it_cli->host + "@" + it_cli->host + " " + *it;
    for(std::list<channel>::iterator to_send = this->_channel_data.begin(); to_send != this->_channel_data.end(); to_send++)
    {
        if (channel_name == to_send->name)
        {
            std::cout << "priv msg channel = |" << *it << "|\n";
            for (std::list<int>::iterator socket_in_channel = to_send->client_socket.begin(); socket_in_channel != to_send->client_socket.end(); socket_in_channel++){
            std::cout << "a envoyer " << *socket_in_channel << std::endl;
        }
            for (std::list<int>::iterator socket_in_channel = to_send->client_socket.begin(); socket_in_channel != to_send->client_socket.end(); socket_in_channel++){
                if (it_cli->socket != *socket_in_channel)
                    send(*socket_in_channel, it->c_str() , it->size(), 0);
            }
        }
    }
    }
    else {
        position = it->find(' ');
        user_to_send = cut_word_space(*it, it->begin() + 8);
        std::cout << "user = |" << user_to_send << "|\n";
        *it = ":" + it_cli->username + "!" + it_cli->host + "@" + it_cli->host + " " + *it;
        for(std::list<clients>::iterator to_send = this->_user_data.begin(); to_send != this->_user_data.end(); to_send++)
        {
            std::cout << "cmd = |" << *it << "|\n";
            if (user_to_send == to_send->username){
                send(to_send->socket, it->c_str() , it->size(), 0);
                break ;
            }
        }
    }
}

void Server::what_cmd(std::list<clients>::iterator it_cli)
{
    std::vector<std::string>::iterator it = this->cmd.begin();

    while (it != cmd.end())
    {
        if (it->find("JOIN ", 0, 5) != std::string::npos)
            commandJOIN(it_cli, it);
        else if (it->find("NICK ", 0, 5) != std::string::npos)
            commandNICK(it_cli, it);
        else if (it->find("USER ", 0, 5) != std::string::npos)
            setup_host(*it, it_cli);
        else if (it->find("PRIVMSG ", 0, 7) != std::string::npos)
            commandPRIVMSG(it_cli, it);    
        else if (it->find("PART ", 0, 5) != std::string::npos)
            commandPART(it_cli, it); 
        it++;
    }
    if (it_cli->nb_msg == 0)
    {
        std::string _wlcmsg = ":127.0.0.1 375 " +  it_cli->username + " ::- 127.0.0.1 Message of the day -\r\n";
		std::string _wlcmsg2 = ":127.0.0.1 376 " +  it_cli->username + " ::End of /MOTD command\r\n";
    
        send(it_cli->socket, _wlcmsg.c_str(), _wlcmsg.size(), 0);
        send(it_cli->socket, _wlcmsg2.c_str(), _wlcmsg2.size(), 0);
        it_cli->nb_msg++;
    }
    return ;
}

void Server::servListen(std::list<pollfd>::iterator it) 
{
	User user;
    std::string temp;
    int rec;
    if(it->revents & POLLIN){
        rec = recv(it->fd, &user.msg, sizeof(user.msg), 0);
        temp.assign(user.msg);
        delete_clrf(temp);
        //ici le vector est setup
        if(rec == 0)
            user_left(it);
		else 
        {
            std::list<clients>::iterator it_cli = this->_user_data.begin();  
            while (it_cli->socket != it->fd)
                it_cli++;
            what_cmd(it_cli);
            it_cli->nb_msg++;
        }
    }
    this->cmd.clear();
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