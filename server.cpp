#include "utils.hpp"

void function(To_send to_send)
{
    
    std::string temp = "Welcome to our IRC ! ;) ";
    int len = temp.size();
    const char *msg = temp.c_str();
    send(to_send.socketClient[to_send.thread], msg, len, 0);
    User user;
    for (int i = 0; i < 5 ; i++)
    {
        recv(to_send.socketClient[to_send.thread], &user, sizeof(User), 0);
        //std::cout << user.msg << std::endl;
        if (0 == to_send.thread)
        {
            send(to_send.socketClient[1], &user, sizeof(user), 0);
            send(to_send.socketClient[2], &user, sizeof(user), 0);
        }
        else if (1 == to_send.thread)
        {
            send(to_send.socketClient[0], &user, sizeof(user), 0);
            send(to_send.socketClient[2], &user, sizeof(user), 0);
        }
        else
        {
            send(to_send.socketClient[1], &user, sizeof(user), 0);
            send(to_send.socketClient[0], &user, sizeof(user), 0);
        }
        strcpy(user.msg, "");
    }
    return ;
}

int main()
{
    int socketServer = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrServer;

    addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(30001);

    bind(socketServer, (const struct sockaddr *)&addrServer, sizeof(addrServer));
    std::cout << "bind ; " << socketServer << std::endl;

    listen(socketServer, 5);
    std::cout << "listen" << std::endl;
    
    int *socketClient = new int[3];
    for(int i = 0; i < 3; i++)
    {
        struct sockaddr_in addrClient;
        socklen_t csize = sizeof(addrClient);
        socketClient[i] = accept(socketServer, (struct sockaddr *)&addrClient, &csize);
        std::cout << "accept" << std::endl;
    }

    for(int i = 0; i < 3; i++)
    {
        To_send to_send;
        to_send.socketClient = socketClient;
        to_send.thread = i;
        std::cout << "client = " << socketClient[i] << std::endl;
        std::thread client (function, to_send);
        client.detach();
    }
    
    while (1);
    std::cout << "Done GG !" << std::endl;
    return 0;
}