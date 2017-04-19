#ifndef TEST_TASK_H__
#define TEST_TASK_H__

#include <iostream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include "nktask.h"

namespace nkserver {

class TaskTest : public NKTask {
public:
    TaskTest() : num__(10), name__("") {}
    ~TaskTest() {}

    int handle() {
        pthread_t pid = pthread_self();
        num__ = pid;
        sleep(1);
        return 0;
    }

    int output() {
        std::cout << "task's num: " << num__ << std::endl;
        return 0;
    }

private:
    unsigned long num__;
    std::string name__;
};

}

#endif //TEST_TASK_H__
