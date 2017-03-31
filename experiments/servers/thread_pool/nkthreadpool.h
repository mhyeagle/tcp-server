#ifndef NK_THREADPOOL_H
#define NK_THREADPOOL_H

#include <memory>
#include <pthread.h>

#include "nkqueue.h"
#include "nktask.h"


class NKThreadpool {
public:
    NKThreadpool(): threads_num__(2) {
        queue__->reserve(100);
		create_workers(2);
    }

    NKThreadpool(int thread_num, int queue_size): threads_num__(thread_num) {
        queue__->reserve(queue_size);
		create_workers(thread_num);
    }

	static void* worker_func(void* arg);

private:
	int create_workers(int size) {
		workers__ = new pthread_t[size];
		int ret = 0;
		for(int i = 0; i < size; ++i) {
			ret = pthread_create(workers+i, NULL, worker_func, NULL);
			if (ret) {
				std::cout << "pthread_create failed." << std::endl;
				break;
			}
		}
		return ret;
	}

private:
    int threads_num__;
    pthread_t* workers__;
    std::shared_ptr<NKQueue<NKTask> > tasks__;
};

#endif //NK_THREADPOOL_H
