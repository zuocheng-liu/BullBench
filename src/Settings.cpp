#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>

#include "BullBench.h"
#include "Settings.h"

void Settings::processParams(int argc, char **argv) {
    int c;
    if (argc < 2) {
        _usage();
        exit(0);
    }
    while ((c = getopt(argc, argv, "hf:H:u:c:t:")) != -1) {
        switch (c) {
            case 'f' :
                fileName.assign(optarg);
                break;
            case 'H' :
                host.assign(optarg);
                break;
            case 'u' :
                urlPrefix.assign(optarg);
                break;
            case 'c' :
                threadNum = atoi(optarg);
                if (threadNum <= 0) {
                    threadNum = DEFAULT_THREAD_NUM;
                }
                break;
            case 'h' :
                _usage();
                exit(0);
        } // end switch
    } // end while

    // parase Domain name
    if (urlPrefix.empty()) {
        std::cerr <<"Fatal: Illegal url prefix" << std::endl;
        exit(-1);
    }
    size_t domainStartPos = std::string::npos;
    size_t domainEndPos = std::string::npos;
    size_t colonsPos = std::string::npos;
    size_t prefixPos = std::string::npos;
    if (std::string::npos == urlPrefix.find("http://")) {
        urlPrefix = "http://" + urlPrefix;
    }
    domainStartPos = 7;
    prefixPos = urlPrefix.find("/", domainStartPos);
    if (std::string::npos != prefixPos) {
        urlPrefix = urlPrefix.substr(0, prefixPos);
    }
    colonsPos = urlPrefix.find(":", domainStartPos);
    // case for http://www.bullsoft.org
    if (std::string::npos == colonsPos) {
        domainEndPos = urlPrefix.length() - 1;
        // case for http://www.bullsoft.org:8080
    } else {
        domainEndPos = colonsPos - 1;
    }
    domainName.assign(urlPrefix.substr(domainStartPos, domainEndPos - domainStartPos + 1));
    if (domainName.empty()) {
        std::cerr <<"Fatal: Illegal url prefix" << std::endl;
        exit(-1);
    }

    // parse Port
    // case for :8080
    if (std::string::npos != colonsPos) {
        size_t portStartPos = colonsPos + 1;
        size_t portEndPos = urlPrefix.length() - 1;
        portString = urlPrefix.substr(portStartPos, portEndPos - portStartPos + 1);
        port = atoi(portString.c_str());
        if (port <= 0) {
            std::cerr <<"Error: Illegal Port:"<< portString << std::endl;
            port = 80;
            portString = "80";
        }
    }

    // get IP adin according to domain name
    struct hostent *hp;
    unsigned long inaddr;
    inaddr = inet_addr(domainName.c_str());
    // case for http://127.0.0.1:8080
    if (inaddr != INADDR_NONE) {
        ip = domainName;
        memcpy(&ad.sin_addr, &inaddr, sizeof(inaddr));
    } else {
        // case for http://www.bullsoft.org
        hp = gethostbyname(domainName.c_str());
        if (hp == NULL) {
            std::cerr <<"Fatal: Invalid domain name:"<< domainName << std::endl;
            exit(-1);
        }
        memcpy(&ad.sin_addr, hp->h_addr, hp->h_length);
    }
    ad.sin_family = AF_INET;
    ad.sin_port = htons((unsigned short)port);
}

void Settings::_usage() {
    std::cout << std::endl << PROGRAM_NAME " " PROGRAM_VERSION 
        << std::endl << "-f <filename> request file path, maybe nginx access log"
        << std::endl << "-t <num>      request file type, 1 for nginx access log, 0 for others , default is 1"
        << std::endl << "-u <url>      request url prefix, unsuport https, such as http://www.bullsoft.org"
        << std::endl << "-H <host>     http request header Host, default is NULL"
        << std::endl << "-c <num>      the number of concurrent threads, default is 100"
        << std::endl << "-h            print this help and exit"
        << std::endl
        //<< std::endl printf("-r            regex expression, used to extract request string from log file \n"); 
        << std::endl << "Examples 1:   ./boolbench -f /var/log/nginx/access.log -u http://127.0.0.1:8080"
        << std::endl << "Examples 2:   ./boolbench -f /var/log/nginx/access.log -u http://127.0.0.1:8080 -H www.bullsoft.org" 
        << std::endl;
}

