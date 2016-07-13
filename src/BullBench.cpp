#include <stdio.h>
#include <pthread.h>

#include <fstream>
#include <iostream>
#include <queue>
#include <vector>

#include "Settings.h"
#include "ThreadPool.h"
#include "BullBenchThread.h"


int main(int argc, char **argv) {
    Settings settings;
    settings.processParams(argc, argv);
    std::queue<std::string> requestQueue;
    
    ThreadPool threadPool;
    for (int i = settings.threadNum; i > 0; --i) {
        threadPool.addThread(new BullBenchThread(settings, requestQueue));
    }
    threadPool.startAll();
       
    int count = 0;
    uint32_t bufferSize = 2 * settings.threadNum;
    std::ifstream sourceFile(settings.fileName.c_str());
    while( ! sourceFile.eof() ) {
        std::string line;
        getline(sourceFile, line);
        if (line.empty() || line.length() < 10 || line.length() > 1024) {
            continue;
        }
        size_t posGET = 0;
        posGET = line.find("GET ");
        if (std::string::npos == posGET) {
            continue;
        }
        size_t posHTTP = 0;
        posHTTP = line.find(" HTTP");
        if (std::string::npos == posHTTP) {
            continue;
        }
        size_t posRequest = posGET + 4;
        std::string requestUri = line.substr(posRequest, posHTTP - posRequest); 
        if (requestUri.empty() || line.length() <= 0) {
            continue;
        }
        ++count;
        if (++count % 100 == 0) {
            std::cout<<"get request uri count:" << count << std::endl;
        }
        pthread_mutex_lock(&settings.mutex);
        if (bufferSize <= requestQueue.size()) {
            pthread_cond_broadcast(&settings.fullCond);
            pthread_cond_wait(&settings.emptyCond, &settings.mutex);
        }
        requestQueue.push(requestUri);
        pthread_mutex_unlock(&settings.mutex); 
    }

    settings.stop = true;
    pthread_cond_broadcast(&settings.fullCond);
    threadPool.stopAll();
    
    std::cout<<std::endl;
    std::cout<<"total request uri get:" << count << std::endl;
    std::cout<<"total request uri sent succ:" << settings.totalSendSucc << std::endl;
    std::cout<<"total request uri sent fail:" << settings.totalSendFail << std::endl;
    sourceFile.close();
    exit(0);
    //threadPool.stopAll();
}
