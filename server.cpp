#include "utils.hpp"


int main()
{
    int socketServer = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrServer;

    addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(30005);

    bind(socketServer, (const struct sockaddr *)&addrServer, sizeof(addrServer));
    std::cout << "bind ; " << socketServer << std::endl;

    listen(socketServer, 5);
    std::cout << "listen" << std::endl;
    
    int *socketClient = new int;
    for(int i = 0; i < 2; i++)
    {
        struct sockaddr_in addrClient;
        socklen_t csize = sizeof(addrClient);
        socketClient[i] = accept(socketServer, (struct sockaddr *)&addrClient, &csize);
        std::cout << "accept" << std::endl;
    }
    User user;
    std::string temp = "Welcome to our IRC ! ;) ";
    int len = temp.size();
    const char *msg = temp.c_str();
    send(socketClient[0], msg, len, 0);
    send(socketClient[1], msg, len, 0);
    struct pollfd fds[2];
    for (int i = 0; i < 2 ; i++){
        fds[i].fd = socketClient[i];
        fds[i].events = POLLIN;
    }
    int e = poll(fds, 2, -1);
    while(1)
    {   
        if (fds[0].revents & POLLIN)
        {    
            std::cout << "non"<< std::endl;
            recv(fds[0].fd, &user, sizeof(User), 0);
            std::cout << user.msg << std::endl;
        }
    }
    std::cout << "test = " << e << std::endl;
    std::cout << user.msg << std::endl;
    while (1);
    std::cout << "Done GG !" << std::endl;
    return 0;
}