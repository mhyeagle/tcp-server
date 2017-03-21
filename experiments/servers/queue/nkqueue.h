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
        pthread_cond_init(&cond__, NULL);
        queue__ = std::make_shared<std::queue<T> >();
    }

    ~NkQueue() {}

    int push(T&);
    int pop(T&);

private:
    pthread_mutex_t mutex__;
    pthread_cond_t cond__;
    std::shared_ptr<std::queue<T> > queue__;
};

template<class T>
int NkQueue<T>::push(T& element) {
    pthread_mutex_lock(&mutex__);
    queue__->push(element);
    pthread_mutex_unlock(&mutex__);
    pthread_cond_signal(&cond__);

    return 0;
}

template<class T>
int NkQueue<T>::pop(T& element) {
    pthread_mutex_lock(&mutex__);
    while(queue__->empty())
        pthread_cond_wait(&cond__, &mutex__);
    element = queue__->front();
    queue__->pop();
    pthread_mutex_unlock(&mutex__);

    return 0;
}

#endif //NK_QUEUE_H

