#ifndef BULLBENCH_THREADPOOL_H
#define BULLBENCH_THREADPOOL_H

#include "Thread.h"

class ThreadPool {
    public :
        typedef std::vector<Thread *> ThreadTypePtrContainer;
        
        ThreadPool() : _threadNum(0), _idle(0) {}

        ~ThreadPool() {}

        inline void addThread(Thread* threadPtr) {
            _threadContainer.push_back(threadPtr);
            ++_threadNum;
        }

        
        inline void startAll() {
            ThreadTypePtrContainer::iterator it;
            for(it = _threadContainer.begin(); it != _threadContainer.end(); ++ it) {
                (*it)->start();
            }
        }

        inline void stopAll() {
            ThreadTypePtrContainer::iterator it;
            for(it = _threadContainer.begin(); it != _threadContainer.end(); ++ it) {
                (*it)->join(NULL);
                //delete *it;
            }
        }
        
    protected :
        ThreadTypePtrContainer _threadContainer;
        u_int32_t _threadNum;
        u_int32_t _idle; // the number of idle threads
}; 

#endif
