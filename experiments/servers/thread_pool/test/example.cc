#include <iostream>
#include <string>
#include <vector>

#include "task_test.h"
#include "nkthreadpool.h"

using namespace std;

int main() {
    NKThreadpool* thread_pool_test = new NKThreadpool(10, 100);
    TaskTest works[10];
    for(int i = 0; i < 10; ++i) {
        thread_pool_test->push(&works[i]);
    }

    TaskTest* task = NULL;
    for(int i = 0; i < 10; ++i) {
        task = NULL;
        thread_pool_test->get_result(task);
        task->output();
    }

    return 0;
}
