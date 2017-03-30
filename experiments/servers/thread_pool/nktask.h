#ifndef NK_TASK_H
#define NK_TASK_H

class NKTask {
public:
    NKTask() {}
    ~NKTask() {}

    virtual int handle() = 0;
};

#endif //NK_TASK_H
