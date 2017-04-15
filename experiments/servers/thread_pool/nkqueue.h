#ifndef NK_QUEUE_H
#define NK_QUEUE_H

#include <vector>
#include <memory>
#include <pthread.h>

#include "nktask.h"

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

int NKQueue::push(NKTask* element) {
    pthread_mutex_lock(&mutex__);
    queue__->push(element);
    pthread_mutex_unlock(&mutex__);
    pthread_cond_signal(&cond__);

    return 0;
}

int NKQueue::pop(NKTask* element) {
    pthread_mutex_lock(&mutex__);
    while(queue__->empty())
        pthread_cond_wait(&cond__, &mutex__);
    size_t size = queue__->size()-1;
    element = (*queue__)[size];
    queue__->erase(size);
    pthread_mutex_unlock(&mutex__);

    return 0;
}

#endif //NK_QUEUE_H

