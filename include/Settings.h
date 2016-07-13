#ifndef BULLBENCH_SETTINGS_H
#define BULLBENCH_SETTINGS_H

#include <string.h>
#include <pthread.h>
#include <netinet/in.h> 

#include <string>

#include "BullBench.h"

class Settings {
    public :
        const static uint32_t DEFAULT_THREAD_NUM = 100;

        std::string fileName; //file which contains request log
        std::string fileType;  
        std::string ip;
        uint16_t port;
        std::string portString;
        std::string serverName;
        std::string host;
        std::string urlPrefix;
        std::string regularExpression;
        std::string domainName;
        uint32_t threadNum;
        struct sockaddr_in ad;
        
        volatile bool stop;
       
        pthread_cond_t fullCond;
        pthread_cond_t emptyCond;
        pthread_mutex_t mutex;
        
        uint32_t totalSendSucc;      
        uint32_t totalSendFail;      

        Settings() : 
            threadNum(DEFAULT_THREAD_NUM), 
            port(80), 
            stop(false),
            totalSendSucc(0),
            totalSendFail(0) {

            portString = "80";
            memset(&ad, 0, sizeof(ad));
            
            pthread_mutex_init(&mutex, NULL);
            pthread_cond_init(&fullCond, NULL);
            pthread_cond_init(&emptyCond, NULL);

        }

        ~Settings() {
            pthread_mutex_destroy(&mutex);
            pthread_cond_destroy(&fullCond);
            pthread_cond_destroy(&emptyCond);
        }
        
        void processParams(int argc, char **argv);       
    private :
        void _usage(); 
}; // end class Settings
#endif
