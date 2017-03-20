#ifndef NK_QUEUE_H
#define NK_QUEUE_H

#include <queue>
#include <memory>
#include <pthread.h>

template<class T>
class NkQueue {
public:
    NkQueue() {
        pthread_mutex_init(&mutex__, NULL);
        queue__ = std::make_shared<std::queue<T> >();
    }

    ~NkQueue() {}

    int push(T&);
    int pop(T&);

private:
    pthread_mutex_t mutex__;
    std::shared_ptr<std::queue<T> > queue__;
};

#endif //NK_QUEUE_H
