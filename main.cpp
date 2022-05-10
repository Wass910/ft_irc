#include "utils.hpp"


int main()
{
    Server server;
    User user;
    while(1)
    {
		int e = poll(server.fds, server._nb_client, -1);
		if(server.fds[0].revents & POLLIN)
			server.add_client_channel();
		for(int x = 1; x < server._nb_client; x++)
            server.send_msg(x);
	}
    return 0;
}