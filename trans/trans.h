//
// Created by liuping on 2019/2/2.
//
#pragma once

class Trans {
public:
    virtual void DoTask(const CoYield &co){};
    virtual ~Trans() {}
};
