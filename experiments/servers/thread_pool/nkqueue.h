#ifndef NK_QUEUE_H
#define NK_QUEUE_H

#include <queue>
#include <memory>
#include <pthread.h>

#include "nktask.h"

class NKQueue {
public:
    NKQueue() {
        pthread_mutex_init(&mutex__, NULL);
        pthread_cond_init(&cond__, NULL);
        queue__ = std::make_shared<std::queue<NKTask*> >();
    }

    ~NKQueue() {}

    int reserve(int size);
    int push(NKTask* task);
    int pop(NKTask* task);

private:
    pthread_mutex_t mutex__;
    pthread_cond_t cond__;
    std::shared_ptr<std::queue<NKTask*> > queue__;
};

int NKQueue::reserve(int size) {
    queue__->reserve(size);
}

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
    element = queue__->front();
    queue__->pop();
    pthread_mutex_unlock(&mutex__);

    return 0;
}

#endif //NK_QUEUE_H

