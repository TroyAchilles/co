#include <stdio.h>
#include <iostream>
#include <vector>

#include "coroutine.h"
#include "co_task.h"

using namespace std;
/*
 * 每个协程的工作函数可以看成是一个事务
 * 这是协程与事务组合的形式使用。
 */

// -------------------- test1 begin ---------------------------
void task_1 (const CoYield &co) {
    std::cout << "task_1 " << 1 <<std::endl;
    co.Yield();
    std::cout << "task_1 " << 2 << std::endl;
    co.Yield();
    std::cout << "task_1 " << 3 << std::endl;
}
void task_2 (const CoYield &co) {
    std::cout << "task_2 " << 1 <<std::endl;
    co.Yield();
    std::cout << "task_2 " << 2 << std::endl;
    co.Yield();
    std::cout << "task_2 " << 3 << std::endl;
}

void Test1() {
    CoTask co_task;
    co_task.AddTack(std::bind(task_1, std::placeholders::_1));
    co_task.AddTack(std::bind(task_2, std::placeholders::_1));
    while (co_task.ResumeAll() > 0);
}
// -------------------- test1 end ---------------------------


// -------------------- test2 begin -------------------------
#include "trans/trans.h"
#include "trans/object_pool.h"

class TransA : public Trans {
public:
    void DoTask(const CoYield &co) final {
        std::cout << co.co_id_ << " task_A " << 1 <<std::endl;
        co.Yield();
        std::cout << co.co_id_ << " task_A " << 2 << std::endl;
        co.Yield();
        std::cout << co.co_id_ << " task_A " << 3 << std::endl;
    }
};

class TransB : public Trans {
public:
    void DoTask(const CoYield &co) final {
        std::cout << co.co_id_ << " task_B " << 1 <<std::endl;
        co.Yield();
        std::cout << co.co_id_ << " task_B " << 2 << std::endl;
        co.Yield();
        std::cout << co.co_id_ << " task_B " << 3 << std::endl;
    }
};

enum CMD {
    TRANS_A = 1,
    TRANS_B = 2
};
void Test2() {
    std::unordered_map<int, ObjectPool<Trans>> trans_mgr;
    for (int i = 0; i < 2; i++) {
        trans_mgr[TRANS_A].add(std::unique_ptr<Trans>((Trans *) new TransA()));
    }

    for (int i = 0; i < 2; i++) {
        trans_mgr[TRANS_B].add(std::unique_ptr<Trans>((Trans *) new TransB()));
    }

    CoTask co_task;
    auto add_task = [&](CMD cmd) {
        auto trans = trans_mgr[cmd].get_shared();
        co_task.AddTack([trans](const CoYield &co) {
            trans->DoTask(co);
        });
    };
    add_task(TRANS_A);
    add_task(TRANS_B);
    add_task(TRANS_A);
    add_task(TRANS_B);
    while (co_task.ResumeAll() > 0);
};

// -------------------- test2 end ---------------------------


int main() {
    Test1();
    Test2();
}
