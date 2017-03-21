#include <iostream>
#include <string>
#include <unistd.h>
#include <pthread.h>

#include "nkqueue.h"

using namespace std;

NkQueue<string> g_queue;

static void* thread_func(void* arg) {
    long num = (long)arg;
    while(1) {
        string str;
        g_queue.pop(str);
        cout << "thread " << num << " get string: " << str << endl;
    }
}

int main() {
    pthread_t tids[20];
    
    for(int i = 0; i < 20; ++i) {
        pthread_create(&tids[i], NULL, thread_func, (void*)i);
    }

    for(int j = 0; j < 100; ++j) {
        sleep(1);
        string str = to_string(j);
        str += " var";
        g_queue.push(str);
    }

    for(int i = 0; i < 20; ++i) {
        pthread_join(tids[i], NULL);
    }

    return 0;
}

