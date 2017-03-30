#ifndef NK_THREADPOOL_H
#define NK_THREADPOOL_H

#include "nkqueue.h"
#include "nktask.h"

class NKThreadpool {
public:
    NKThreadpool(): threads_num__(2) {
        queue__->reserve(100);
        workers->new pthread_t[2];
        for(int i = 0; i < 2; ++i) {
            
        }
    }

    NKThreadpool(int thread_num, int queue_size): threads_num__(thread_num) {
        queue__->reserve(queue_size);
    }

    int run(NKTask* task);
    int run(std::vector<NKTask*>& tasks);

private:
    int threads_num__;
    std::shared_ptr<> workers__;
    std::shared_ptr<NKQueue<NKTask> > tasks__;
};

#endif //NK_THREADPOOL_H
