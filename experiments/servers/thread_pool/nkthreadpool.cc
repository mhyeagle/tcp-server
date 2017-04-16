#include "nkthreadpool.h"

namespace nkserver {

NKThreadpool::~NKThreadpool() {

}

int NKThreadpool::create_workers(int size) {
	pthread_t threadid;
	int ret = 0;
	for(int i = 0; i < size; ++i) {
		ret = pthread_create(&threadid, NULL, worker_func, NULL);
		if (ret) {
			std::cout << "pthread_create failed." << std::endl;
			exit(1);
		}
        workers__.push_back(threadid);
	}
	return ret;
}

void* NKThreadpool::worker_func(void* arg) {
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

	pthread_exit();
}

}


