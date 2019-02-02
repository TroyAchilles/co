# 协程的一个简单封装
* 使用方式
```cpp

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

void main() {
    CoTask co_tack;
    co_tack.AddTack(std::bind(task_1, std::placeholders::_1));
    co_tack.AddTack(std::bind(task_2, std::placeholders::_1));
    while (co_tack.ResumeAll() > 0);
}
/* 输出
task_2 1
task_1 1
task_2 2
task_1 2
task_2 3
task_1 3
*/
```

* 带事务桶的使用方式
```cpp

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
void main() {
    std::unordered_map<int, ObjectPool<Trans>> trans_mgr; // 事务桶
    // 向桶中添加两个transA、两个transB
    for (int i = 0; i < 2; i++) {
        trans_mgr[TRANS_A].add(std::unique_ptr<Trans>((Trans *) new TransA()));
    }

    for (int i = 0; i < 2; i++) {
        trans_mgr[TRANS_B].add(std::unique_ptr<Trans>((Trans *) new TransB()));
    }

    CoTask co_task;

    // 取得一个事务类，将它对应的任务添加到co_task中
    auto add_task = [&](CMD cmd) {
        auto trans = trans_mgr[cmd].get_shared();
        co_task.AddTack([trans](const CoYield &co) { //trans为共享指针 当lambda对象析构的时候会将事务放回trans_mgr
            trans->DoTask(co);
        });
    };
    add_task(TRANS_A);
    add_task(TRANS_B);
    add_task(TRANS_A);
    add_task(TRANS_B);

    while (co_task.ResumeAll() > 0);

/*
4 task_B 1
3 task_A 1
2 task_B 1
1 task_A 1
4 task_B 2
3 task_A 2
2 task_B 2
1 task_A 2
4 task_B 3
3 task_A 3
2 task_B 3
1 task_A 3
*/
};
```
