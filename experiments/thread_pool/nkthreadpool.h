#ifndef NK_THREADPOOL_H
#define NK_THREADPOOL_H

#include <memory>
#include <pthread.h>

#include "nkqueue.h"
#include "nktask.h"

namespace nkserver {

class NKThreadpool {
public:
    NKThreadpool(): threads_num__(10), shutdown__(false) {
        tasks__ = std::make_shared<NKQueue>(1000);
        if (tasks__ == NULL) {
            std::cout << "create tasks queue failed." << std::endl;
        }
        results__ = std::make_shared<NKQueue>(1000);
        if (results__ == NULL) {
            std::cout << "create results queue failed." << std::endl;
        }
        create_workers(10);
    }

    NKThreadpool(int thread_num, int queue_size = 100): threads_num__(thread_num), shutdown__(false) {
        tasks__ = std::make_shared<NKQueue>(queue_size);
        if (tasks__ == NULL) {
            std::cout << "create tasks queue failed." << std::endl;
        }
        results__ = std::make_shared<NKQueue>(queue_size);
        if (results__ == NULL) {
            std::cout << "create results queue failed." << std::endl;
        }
        create_workers(thread_num);
    }

    virtual ~NKThreadpool();

    static void* worker_func(void* arg);
    void process();

    int push_task(NKTask* task);
    int get_result(NKTask** task);
    void shutdown();

private:
    int create_workers(int size);

private:
    int threads_num__;
    bool shutdown__;
    std::vector<pthread_t> workers__;
    std::shared_ptr<NKQueue> tasks__;
    std::shared_ptr<NKQueue> results__;
};

}

#endif //NK_THREADPOOL_H
