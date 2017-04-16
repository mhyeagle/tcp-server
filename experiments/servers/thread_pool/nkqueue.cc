#include "nkqueue.h"

namespace nkserver {

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

}
