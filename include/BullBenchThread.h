#ifndef BULLBENCH_BULLBENCHTHREAD_H
#define BULLBENCH_BULLBENCHTHREAD_H

#include <sys/socket.h>
#include <netinet/tcp.h> // for TCP_NODELAY

#include <queue>

#include "Thread.h"
#include "Settings.h"

class BullBenchThread : public Thread {
    public:
        BullBenchThread(Settings& settings, std::queue<std::string>& requestQueue) 
            : _settings(settings), _requestQueue(requestQueue) {}
        ~BullBenchThread() {}
        
        void run(); 
    private :
       inline int _getSocket() {
            int sock;
            sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) {
                return -1;
            }

            // avoid TIME_WAIT status
            int flags =1;
            setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(flags));
            struct linger ling = {0, 0};
            setsockopt(sock, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
            
            // setsockopt(sfd, SOL_SOCKET, SO_KEEPALIVE, &flags, sizeof(flags));
            setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &flags, sizeof(flags));

            if (connect(sock, (struct sockaddr *)&(_settings.ad), sizeof(_settings.ad)) < 0) {
                return -1;
            }
            return sock;
        }

        void buildRequest(char* request,std::string& uri); 
        
        Settings& _settings;
        std::queue<std::string>& _requestQueue;
};
// end class BullBenchThread
#endif
