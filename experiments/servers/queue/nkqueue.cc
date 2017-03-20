#include "nkqueue.h"

int NkQueue::push(T& element) {
    pthread_mutex_lock(&mutex__);
    queue__->push(element);
    pthread_mutex_unlock(&mutex__);
}

int NkQueue::pop(T& element) {

}
