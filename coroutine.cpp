
#include "coroutine.h"

int CoPool::NewCoroutine(func_hander func, task_type task, void *arg) {
    if(free_coroutine.empty()){
        inc_co_id++;
        Coroutine *co = new Coroutine();
        co->co_id = inc_co_id;
        free_coroutine.push_back(co);
    }

    Coroutine *co = free_coroutine.front();
    free_coroutine.pop_front();
    if(co == nullptr)
        return -1;

    getcontext(&co->context);
    co->context.uc_stack.ss_sp = co->stack;
    co->context.uc_stack.ss_size = sizeof(co->stack);
    co->context.uc_link = &main;
    co->task = std::move(task);

    makecontext(&co->context, (void (*)())func, 3, this, co, arg);
    action_coroutine[co->co_id] = co;
    return co->co_id;
}

int CoPool::Yield(int co_id){
    Coroutine *co = FindCoId(co_id);
    if (co == nullptr)
        return -1;

    swapcontext(&co->context, &main);
    return 0;
}

int CoPool::Resume(int co_id){
    Coroutine *co = FindCoId(co_id);
    if (co == nullptr)
        return -1;
    swapcontext(&main, &co->context);
    return 0;
}

void CoPool::FreeCoroutine(int co_id) {
    auto iter = action_coroutine.find(co_id);
    if(iter == action_coroutine.end()) { return; }
    free_coroutine.push_back(iter->second);
    action_coroutine.erase(iter);
}

Coroutine* CoPool::FindCoId(int co_id) {
    auto iter = action_coroutine.find(co_id);
    return iter == action_coroutine.end() ? nullptr : iter->second;
}

const ActionCo & CoPool::GetActionCo() {
    return action_coroutine;
}
