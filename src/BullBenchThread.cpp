#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <queue>
#include <iostream>
#include "BullBenchThread.h"
#include "BullBench.h"

#define REQUEST_SIZE 2048

void BullBenchThread::run() {
    int succ = 0;
    for (;;) {
        pthread_mutex_lock(&_settings.mutex);
        if (succ > 0) _settings.totalSendSucc ++;
        if (succ < 0) _settings.totalSendFail ++;
        if (succ > 0 && _settings.totalSendSucc % 1000 == 0) {
            std::cout<<"send request uri count:" << _settings.totalSendSucc << std::endl;
        }
        if (_requestQueue.empty()) {
            if (_settings.stop) {
                pthread_mutex_unlock(&_settings.mutex);
                exit(NULL);
            }
            pthread_cond_broadcast(&_settings.emptyCond);
            pthread_cond_wait(&_settings.fullCond, &_settings.mutex);
            pthread_mutex_unlock(&_settings.mutex);
            continue;
        }

        std::string url = _requestQueue.front();
        _requestQueue.pop();
        pthread_mutex_unlock(&_settings.mutex);

        char request[REQUEST_SIZE];
        buildRequest(request, url);
        int rlen = strlen(request);

        int sock = _getSocket();
        if (sock < 0) {
            std::cerr << "Error: sock fail :" 
                << _settings.domainName << ':' << _settings.port 
                << std::endl;
            succ = -1;
            continue;
        }
        if(rlen!=write(sock,request,rlen)) {
            close(sock);
            std::cerr<<"Error: write fail :" << std::endl;
            succ = -1;
            continue;
        }
        // Read all
        char response[8192];
        while (read(sock,response,8192) > 0) {
            // do nothing;
        }
        close(sock);
        succ = 1;
    } // end for
}

void BullBenchThread::buildRequest(char* request,std::string& uri) {
            //Five_comment: same as memset bytes to sezo('\0')
            bzero(request,REQUEST_SIZE);
            
            strcpy(request,"GET");
            strcat(request," ");
            
            if (_settings.host.empty()) {
                strcat(request, (char *)uri.c_str());
            } else {
                strcat(request, (char *)_settings.urlPrefix.c_str());
                strcat(request, (char *)uri.c_str());
            }

            // detemine which http version
            strcat(request," HTTP/1.1");
            strcat(request,"\r\n");
            strcat(request,"User-Agent: "PROGRAM_NAME" "PROGRAM_VERSION"\r\n");
            strcat(request,"Host: ");
            if (_settings.host.empty()) {
                if (80 == _settings.port) {
                    strcat(request, _settings.domainName.c_str());
                } else {
                    strcat(request, (_settings.domainName + ":" + _settings.portString).c_str());
                }
            } else {
                strcat(request, _settings.host.c_str());
            }
            strcat(request,"\r\n");
            strcat(request,"Pragma: no-cache\r\n");
            strcat(request,"Connection: close\r\n");
            //strcat(request,"Connection: keep-alive\r\n");
            strcat(request,"Keep-Alive: timeout=20\r\n");
            strcat(request,"\r\n"); 
        }

