#ifndef NK_QUEUE_H
#define NK_QUEUE_H

#include <vector>
#include <memory>
#include <pthread.h>

#include "nktask.h"

namespace nkserver {

class NKQueue {
public:
    NKQueue() {
        //pthread_mutex_init(&mutex__, NULL);
        //pthread_cond_init(&cond__, NULL);
        queue__ = std::make_shared<std::vector<NKTask*> >();
    }

    NKQueue(int size) {
        //pthread_mutex_init(&mutex__, NULL);
        //pthread_cond_init(&cond__, NULL);
        queue__ = std::make_shared<std::vector<NKTask*> >();
        queue__->reserve(size);
    }

    ~NKQueue() {}

    int push(NKTask* task);
    int pop(NKTask* task);

private:
    pthread_mutex_t mutex__ = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond__ = PTHREAD_COND_INITIALIZER;
    std::shared_ptr<std::vector<NKTask*> > queue__;
};

}

#endif //NK_QUEUE_H

