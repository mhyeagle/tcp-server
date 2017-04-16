#ifndef TEST_TASK_H__
#define TEST_TASK_H__

#include <unistd.h>
#include "nktask.h"

using namespace nkserver;

class TaskTest : public NKTask {
public:
    TaskTest() : num__(10), name__("") {}
    ~TaskTest();

    int handle() {
        usleep(100000);
        return 0;
    }

private:
    int num__;
    std::string name__;
};

#endif //TEST_TASK_H__
