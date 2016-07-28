#include <stdio.h>
#include <pthread.h>
#include <sys/resource.h> 
#include <errno.h>

#include <fstream>
#include <iostream>
#include <queue>
#include <vector>

#include "Settings.h"
#include "ThreadPool.h"
#include "BullBenchThread.h"

void getUriFromNginxLog(std::string& line, std::string& uri) {
    if (line.empty() || line.length() < 10 || line.length() > 1500) {
        uri.clear();
        return;
    }
    size_t posGET = 0;
    posGET = line.find("GET ");
    if (std::string::npos == posGET) {
        uri.clear();
    }
    size_t posHTTP = 0;
    posHTTP = line.find(" HTTP");
    if (std::string::npos == posHTTP) {
        uri.clear();
    }
    size_t posRequest = posGET + 4;
    uri = line.substr(posRequest, posHTTP - posRequest); 
}

void getUriAccordingToRegExp(regex_t& regexCompiled, std::string& line, std::string& uri) {
    if (line.empty() || uri.empty()) {
        return;
    }

    uint32_t maxGroups = 10;
    regmatch_t groupArray[maxGroups];
    char dollar[] = "$0";
    const char * str = line.c_str();
    if (regexec(&regexCompiled, str, maxGroups, groupArray, 0) == 0) {
        for (uint32_t g = 0; g < maxGroups; g++) {
            if (groupArray[g].rm_so == -1) {
                break; // No more groups
            }
            dollar[1] = '0' + g;
            for (;;) {
                size_t dollarPos = uri.find(dollar);
                if (std::string::npos == dollarPos) {
                    break;
                }
                uri = uri.replace(dollarPos, strlen(dollar), str + groupArray[g].rm_so, groupArray[g].rm_eo - groupArray[g].rm_so);
            }
        } // end for
    }
}

int main(int argc, char **argv) {
    struct rlimit rlim;
    if (getrlimit(RLIMIT_NOFILE, &rlim) < 0) {
        std::cerr<<"Can not get rlimit, " << strerror(errno) << std::endl;
        exit(-1);
    }
    rlim.rlim_cur = rlim.rlim_max;
    if (setrlimit(RLIMIT_NOFILE, &rlim) < 0) {
        std::cerr<<"Can not set rlimit, " << strerror(errno) << std::endl;
        exit(-1);
    } 

    Settings settings;
    settings.processParams(argc, argv);
    std::queue<std::string> requestQueue;
      
    std::ifstream sourceFile(settings.fileName.c_str());
    if (! sourceFile.is_open()) {
        std::cerr<<"Can not open file:" << settings.fileName << std::endl;
        exit(-1);
    }

    int count = 0;
    uint32_t bufferSize = 2 * settings.threadNum;
    ThreadPool threadPool;
    for (int i = settings.threadNum; i > 0; --i) {
        threadPool.addThread(new BullBenchThread(settings, requestQueue));
    }
    threadPool.startAll();
    
    settings.startTimer();
    while( ! sourceFile.eof() ) {
        std::string line;
        getline(sourceFile, line);
        if (line.empty()) {
            continue;
        }
        std::string requestUri;
        switch (settings.fileType) {
            default :
            case Settings::NGINX_ACCESS_LOG :
                getUriFromNginxLog(line, requestUri);
                break;
            case Settings::OTHER_FILE_TYPE :
                requestUri = settings.replacementUri;
                getUriAccordingToRegExp(settings.reg, line, requestUri); 
                break;
        }
        if (requestUri.empty()) {
            continue;
        }
        pthread_mutex_lock(&settings.mutex);
        if (bufferSize <= requestQueue.size()) {
            pthread_cond_broadcast(&settings.fullCond);
            pthread_cond_wait(&settings.emptyCond, &settings.mutex);
        }
        requestQueue.push(requestUri);
        pthread_mutex_unlock(&settings.mutex); 
        ++ count;
        if (count % 1000 == 0) {
            std::cout<<"get request uri count:" << count << std::endl;
        }
    }

    pthread_mutex_lock(&settings.mutex);
    settings.stop = true;
    pthread_cond_broadcast(&settings.fullCond);
    pthread_mutex_unlock(&settings.mutex); 
    threadPool.stopAll();
    
    std::cout<<std::endl;
    std::cout<<"total request uri get:" << count << std::endl;
    std::cout<<"total request uri sent succ:" << settings.totalSendSucc << std::endl;
    std::cout<<"total request uri sent fail:" << settings.totalSendFail << std::endl;
    uint64_t time = settings.getTimeCost();
    std::cout<<"total time cost: " << time / 1000000 <<" seconds, " 
        << time % 1000000 << " microseconds" <<std::endl;
    sourceFile.close();
    exit(0);
    //threadPool.stopAll();
}
