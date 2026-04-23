#pragma once

#include <iostream>
using std::cout;
using std::endl;

// malloc 本身就是一个内存池，不是直接从堆上申请内存的
// linux：brk, mmap
// win32：VirtualAlloc

template<class T>
class fixedPool{
private:
    char* _memory; // 一整块内存
    size_t _remain_bytes;
    void* _freelist; // 返还回来的内存链表

public:
    fixedPool()
    :_memory(nullptr), _remain_bytes(0), _freelist(nullptr)
    {}

    T* myNew(){
        // cout << "myNew()" << endl;
        T* obj = nullptr;
        size_t obj_size = sizeof(T);

        // 如果我当前的_freelist非空，优先使用这个
        if(_freelist){
            // 将_freelist指向的那个指针，解释为指向指针的指针，就是这个内存块中存储的下一个位置的指针
            void* next = *(void**)_freelist;
            obj = (T*)_freelist;
            _freelist = next;
        }

        // 剩余内存不够开空间了，直接舍弃那一小部分，重开内存
        else
        {
            if (_remain_bytes < obj_size)
            {
                _remain_bytes = 8 * 1024; // 8KB
                _memory = (char *)malloc(_remain_bytes);

                if (!_memory)
                    throw std::bad_alloc();
            }
            // 够开
            // obj 指向_memory
            obj = (T *)_memory;
            // 32/64位下，保证这个内存空间至少能存储一个指针大小，才能去构建链表结构
            if(obj_size < sizeof(void*))
                obj_size = sizeof(void*);
            _memory += obj_size;
            _remain_bytes -= obj_size;
        }
        // 显示定位new，复用已有内存，不重新开辟
        new(obj) T();
        return obj;
    }

    void myDelete(T* obj){
        // cout << obj << " delete()" << endl;
        // 显示调用析构函数，不将内存归还给系统，放到内存池中去
        obj->~T();
        // 归还内存，直接挂载到_freelist上面
        // 头插，obj的前4/8个字节写入下一个指针的地址
        *(void**)obj = _freelist;
        _freelist = obj;
    }

    inline size_t myGetRemainBytes(){
        return _remain_bytes;
    }

    ~fixedPool() = default;
};


// struct TreeNode
// {
// 	int _val;
// 	TreeNode* _left;
// 	TreeNode* _right;

// 	TreeNode()
// 		:_val(0)
// 		, _left(nullptr)
// 		, _right(nullptr)
// 	{}
// };


// #include <vector>

// void TestObjectPool()
// {
//     // fixedPool<int> pool;
//     // std::vector<int*> v;
//     // // 模拟申请10次
//     // for(int i = 0; i < 10; ++i){
//     //     cout << "bytes: " << pool.myGetRemainBytes() << endl;
//     //     v.push_back(pool.myNew());
//     //     cout << v[i] << endl;
//     // }
//     // for(int i = 0; i < 10; ++i)
//     //     pool.myDelete(v[i]);
// 	// 申请释放的轮次
// 	const size_t Rounds = 10;

// 	// 每轮申请释放多少次
// 	const size_t N = 100000;

// 	std::vector<TreeNode*> v1;
// 	v1.reserve(N);

// 	size_t begin1 = clock();
// 	for (size_t j = 0; j < Rounds; ++j)
// 	{
// 		for (int i = 0; i < N; ++i)
// 		{
// 			v1.push_back(new TreeNode);
// 		}
// 		for (int i = 0; i < N; ++i)
// 		{
// 			delete v1[i];
// 		}
// 		v1.clear();
// 	}

// 	size_t end1 = clock();

// 	std::vector<TreeNode*> v2;
// 	v2.reserve(N);

// 	fixedPool<TreeNode> TNPool;
// 	size_t begin2 = clock();
// 	for (size_t j = 0; j < Rounds; ++j)
// 	{
// 		for (int i = 0; i < N; ++i)
// 		{
// 			v2.push_back(TNPool.myNew());
// 		}
// 		for (int i = 0; i < N; ++i)
// 		{
// 			TNPool.myDelete(v2[i]);
// 		}
// 		v2.clear();
// 	}
// 	size_t end2 = clock();

// 	cout << "new cost time:" << end1 - begin1 << endl;
// 	cout << "object pool cost time:" << end2 - begin2 << endl;
// }