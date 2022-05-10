#include "utils.hpp"


int main()
{
    int socketServer = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrServer;

    addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(30023);

    bind(socketServer, (const struct sockaddr *)&addrServer, sizeof(addrServer));
    std::cout << "bind ; " << socketServer << std::endl;

    listen(socketServer, 5);
    std::cout << "listen" << std::endl;

    User user;
    std::string temp = "Welcome to our IRC ! ;) ";
    int len = temp.size();
    const char *msg = temp.c_str();

	int i = 1;
    Server server(socketServer);
    while(1)
    {
		int e = poll(server.fds, server._nb_client, -1);
		if(server.fds[0].revents & POLLIN){
			server.add_client_channel();
		}
		for(int x = 1; x < server._nb_client; x++){
        if(server.fds[x].revents & POLLIN){
            if(recv(server.fds[x].fd, &user, sizeof(User), 0) == 0){
				std::cout << "USER: " << server.fds[x].fd << " disconnected." << std::endl;
				close(server.fds[x].fd);
                server._nb_client_channel--;
                if (server._nb_client_channel == 0)
                {
                    std::cout << "channel close \n";
                    exit(0);
                }
			}
			else
            {
                std::cout << user.msg << std::endl;
               
            } 
            	
        }
		}
	}
    return 0;
}