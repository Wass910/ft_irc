#include "utils.hpp"

int main()
{
    int socketClient = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrClient;

    addrClient.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrClient.sin_family = AF_INET;
    addrClient.sin_port = htons(30011);

    connect(socketClient, (const struct sockaddr *)&addrClient, sizeof(addrClient));
    std::cout << "Connecte" << std::endl;



    char msg[500];
    recv(socketClient, &msg, 500, 0);
    std::cout << msg << std::endl;
    std::string temp;
    User user;
    // struct pollfd fds[1];
	// int i = 1;
	// fds[0].fd = socketClient;
	// fds[0].events = POLLIN;
    getline(std::cin, temp);
    user.len = temp.size();
    strcpy(user.msg, temp.c_str());
    send(socketClient, &user, sizeof(User), 0);
    char msg2[50];
    recv(socketClient, &msg2, 50, 0);
    std::cout << msg2 << std::endl;
    while(1)
    {
        // poll(fds, i, 10);
        // if(fds[0].revents & POLLIN)
        // {
        //     recv(fds[0].fd, &user, sizeof(User), 0);
        //     std::cout << user.msg << std::endl;
        // }
        getline(std::cin, temp);
        user.len = temp.size();
        strcpy(user.msg, temp.c_str());
        send(socketClient, &user, sizeof(User), 0);
        temp.replace(0, temp.size(), "");
    }

    return 0;
}