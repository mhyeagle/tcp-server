#include <iostream>
#include "nkthreadpool.h"

namespace nkserver {

NKThreadpool::~NKThreadpool() {
    shutdown();
}

void NKThreadpool::shutdown() {
    shutdown__ = true;
    for(int i = 0; i < threads_num__; ++i) {
        pthread_join(workers__[i], NULL);
    }
}

int NKThreadpool::create_workers(int size) {
    pthread_t threadid;
    int ret = 0;
    for(int i = 0; i < size; ++i) {
        ret = pthread_create(&threadid, NULL, worker_func, this);
        if (ret) {
            std::cout << "pthread_create failed." << std::endl;
            exit(1);
        }
        workers__.push_back(threadid);
    }
    return ret;
}

void* NKThreadpool::worker_func(void* arg) {
    NKThreadpool* p_threadpool = static_cast<NKThreadpool*>(arg);
    p_threadpool->process();
    
    return 0;
}

void NKThreadpool::process() {
    while(shutdown__ == false) {
        NKTask* task = NULL;
        int ret = tasks__->pop(task);
        if (ret) {
            //std::cout << "get task from tasks__ failed." << std::endl;
            continue;
        }

        ret = task->handle();
        if (ret) {
            //std::cout << "task failed." << std::endl;
            continue;
        }
        
        ret = results__->push(task);
        if (ret) {
            //std::cout << "push finished task to results__ failed." << std::endl;
            continue;
        }
    }

    pthread_exit(NULL);
}

int NKThreadpool::push_task(NKTask* task) {
    int ret = 0;
    ret = tasks__->push(task);
    if (ret) {
        //std::cout << "push to tasks__ failed." << std::endl;
    }

    return ret;
}

int NKThreadpool::get_result(NKTask* task) {
    int ret = 0;
    ret = results__->pop(task);
    if (ret) {
        //std::cout << "pop from results__ failed." << std::endl;
    }

    return ret;
}

}


