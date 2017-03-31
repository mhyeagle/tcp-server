#include "nkthreadpool.h"

void* NKThreadpool::worker_func(void* arg) {
	NKTask* task = queue__->pop();
	task->process();
}
