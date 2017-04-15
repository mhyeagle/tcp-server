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
		create_workers(10);
        tasks__ = make_shared<NKQueue>(1000);
    }

    NKThreadpool(int thread_num, int queue_size): threads_num__(thread_num), shutdown__(false) {
		create_workers(thread_num);
        tasks__ = make_shared<NKQueue>(queue_size);
    }

	static void* worker_func(void* arg);

private:
	int create_workers(int size);

private:
    bool shutdown__;
    int threads_num__;
    std::vector<pthread_t> workers__;
    std::shared_ptr<NKQueue> tasks__;
};

int NKThreadpool::create_workers(int size) {
	pthread_t threadid;
	int ret = 0;
	for(int i = 0; i < size; ++i) {
		ret = pthread_create(&threadid, NULL, worker_func, NULL);
		if (ret) {
			std::cout << "pthread_create failed." << std::endl;
			break;
		}
        workers__.push_back(threadid);
	}
	return ret;
}

void* NKThreadpool::worker_func(void* arg) {
    while(1) {
	    NKTask* task = queue__->pop();
	    task->handle();
    }
}

}

#endif //NK_THREADPOOL_H
