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


typedef struct _Server{
    int                 socketClient;
    struct sockaddr_in  addrClient;
    socklen_t           csize;
    std::string         nickname;
    int                 nb_msg;

} _Server;

class Server {
    public:
        Server( void );
        Server(std::string const & name, int level);
        Server( Server const & src );
        ~Server(void);

        
        Server & operator=( Server const & src );
        void add_client_channel( void );
        void send_msg( int x );
        int get_nb_client(void);
        int get_nb_client_channel(void);
        struct pollfd* get_fds( void );
        void new_fds( int x );
        void new_vector( int x );
        
    private:
        std::vector<_Server> inf_client;
        int     _nb_client;
        int     _nb_client_channel;
        struct pollfd _fds[10];

        
        
};

typedef struct User{
    int     len;
    char    msg[500];
} User;

