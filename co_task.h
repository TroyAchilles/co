//
// Created by admin on 2018/12/19.
//
#pragma once

#include <unordered_set>
#include <functional>

#include "coroutine.h"

class CoYield {
    friend class CoTask;
    CoPool &co_pool_;
public:
    int32_t co_id_;
    CoYield(CoPool &co_pool, int32_t co_id) : co_pool_(co_pool), co_id_(co_id) {
    }
    ~CoYield() {
        co_pool_.FreeCoroutine(co_id_);
    }
    int Yield() const {
        return co_pool_.Yield(co_id_);
    }
};

using TaskFreeCall = std::function<void (int)>;
class CoTask {
public:
    CoTask () {}

    int AddTack(task_type && task);

    // 通过co_id唤醒
    int ResumeOne(int co_id);

    // 返回本次resume的个数
    int ResumeAll();

private:
    static void Function(void *co_pool, void *co, void *co_task);

    CoPool co_pool_;
};
