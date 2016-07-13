#ifndef BULLBENCH_THREAD_H
#define BULLBENCH_THREAD_H

#include <pthread.h>
#include <cstdlib>
#include <iostream>

class Thread {
    public :
        Thread():_tid(0) {}
        virtual ~Thread() {}
        inline void start() {
            int res = pthread_create(&_tid, NULL, _StartThread, (void *)this);
            if (res != 0) {
                std::cerr << "Fatal: thread created fail!" << std::endl;
                ::exit(-1);
            }
        }

        inline pthread_t getId() {return _tid; }
        inline int join(void **status = NULL) {
            return pthread_join(_tid, status);
        }

        inline int detach() {
            return pthread_detach(_tid);
        }

        inline void exit(void *status = NULL) {
            pthread_exit(status);
        }
        virtual void run() = 0;
    
    protected :
        static void * _StartThread(void *arg) {
            Thread *ptr = (Thread *)arg;
            ptr->run();
            return NULL;
        }

        pthread_t _tid;
}; // end class Thread
#endif
