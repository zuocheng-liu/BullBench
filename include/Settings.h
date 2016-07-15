#ifndef BULLBENCH_SETTINGS_H
#define BULLBENCH_SETTINGS_H

#include <string.h>
#include <pthread.h>
#include <netinet/in.h> 
#include <regex.h> 
#include <sys/time.h> 

#include <string>

#include "BullBench.h"

class Settings {
    public :
        const static uint32_t DEFAULT_THREAD_NUM = 100;
        
        typedef enum {
            OTHER_FILE_TYPE = 0,
            NGINX_ACCESS_LOG = 1
        } FileType;

        std::string fileName; //file which contains request log
        uint32_t fileType;  
        std::string ip;
        uint16_t port;
        std::string portString;
        std::string serverName;
        std::string host;
        std::string urlPrefix;
        std::string domainName;
        uint32_t threadNum;
        struct sockaddr_in ad;
       
        std::string regularExpression;
        std::string replacementUri;
        regex_t reg;

        volatile bool stop;
       
        pthread_cond_t fullCond;
        pthread_cond_t emptyCond;
        pthread_mutex_t mutex;
        
        uint32_t totalSendSucc;      
        uint32_t totalSendFail;      
        
        uint64_t timer;

        Settings() : 
            fileName(""),
            fileType(NGINX_ACCESS_LOG),
            ip(""),
            port(80),
            portString("80"),
            serverName(""),
            host(""),
            urlPrefix(""),
            domainName(""),
            threadNum(DEFAULT_THREAD_NUM), 
            stop(false),
            totalSendSucc(0),
            totalSendFail(0) {

            memset(&ad, 0, sizeof(ad));
            
            pthread_mutex_init(&mutex, NULL);
            pthread_cond_init(&fullCond, NULL);
            pthread_cond_init(&emptyCond, NULL);
        }

        ~Settings() {
            pthread_mutex_destroy(&mutex);
            pthread_cond_destroy(&fullCond);
            pthread_cond_destroy(&emptyCond);
            
            regfree(&reg);
        }
        
        void processParams(int argc, char **argv);
        inline void startTimer() {
            timeval tv;
            gettimeofday(&tv,NULL);
            timer = 1000000 * tv.tv_sec + tv.tv_usec;
        }
        inline uint64_t getTimeCost() {
            timeval tv;
            gettimeofday(&tv,NULL);
            return 1000000 * tv.tv_sec + tv.tv_usec - timer;
        } 
    private :
        void _usage(); 
}; // end class Settings
#endif
