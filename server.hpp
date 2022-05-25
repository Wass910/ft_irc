#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <cctype>
#include <string>
#include <vector>
#include <thread>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <list>

# define JOIN 1
# define QUIT 2

typedef struct User{
    int     len;
    char    msg[500];
} User;

typedef struct clients{
	struct sockaddr_in addrClient;
	socklen_t csize;
	int socket;
	int nb_msg;
	std::string username;
	std::string password;
	std::string name;
	std::string channel;
	std::string host;
}clients;

typedef struct channel{
	int nb_client;
	std::string name;
}channel;

class Server{
	public:
		Server();
		Server( int port);
		~Server();

		void addUser();
		void servListen(std::list<pollfd>::iterator it);
		void update_revents();
		struct pollfd *get_fds();
		std::list<pollfd> get_lfds();
		
	private:
		struct pollfd _fds[100];
		struct sockaddr_in _addrServer;

		void build_fds();
		void display_fds();
		void setup_username( std::string nickname, std::list<clients>::iterator it_cli, int first);
		void setup_password( std::string password, std::list<clients>::iterator it_cli);
		void commandPART(std::list<clients>::iterator it_cli);
		void setup_host( std::string host, std::list<clients>::iterator it_cli );
		void user_left( std::list<pollfd>::iterator it );
		bool channel_open(std::string channel_name);
		void channel_empty(std::string channel_name);
		void create_channel(int user, std::list<clients>::iterator it_cli, std::string msg);
		void delete_clrf(std::string temp);
		void what_cmd(std::list<clients>::iterator it_cli);
		std::string cut_word_space( std::string to_cut, std::string::iterator it );
		int _clients;
		int _serverSocket;

		std::string _wlcmsg = ":127.0.0.1 375 user42 ::- 127.0.0.1 Message of the day -\r\n";
		std::string _wlcmsg2 = ":127.0.0.1 376 user42 ::End of /MOTD command\r\n";
		std::list<pollfd> _lfds;
		std::list<clients> _user_data;
		std::list<channel> _channel_data;
		std::vector<std::string> cmd;
};

#endif //SERVER_H
