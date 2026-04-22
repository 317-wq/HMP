#pragma once

#include <iostream>
using std::cout;
using std::endl;

template<class T>
class fixedPool{
private:
    char* _memory; // 一整块内存
    void* _freelist; // 返还回来的内存列表，申请的数据类型也不知道
public:
    fixedPool()
    :_memory(nullptr), _freelist(nullptr)
    {}
};