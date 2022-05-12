#include "utils.hpp"

int main()
{
    Server server;
    User user;
    while(1)
    {
		poll(server.get_fds(), server.get_nb_client(), -1);
		if(server.get_fds()[0].revents & POLLIN)
			server.add_client_channel();
		for(int x = 1; x < server.get_nb_client(); x++)
            server.send_msg(x);
	}
    return 0;
}