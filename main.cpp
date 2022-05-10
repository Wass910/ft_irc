#include "utils.hpp"


int main()
{
    Server server;
    User user;
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