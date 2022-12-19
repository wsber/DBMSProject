//
// Created by hp on 2022/10/11.
//

#ifndef DBMS_PF_HASHTABEL_H
#define DBMS_PF_HASHTABEL_H

#include "pf.h"

struct PF_HashEntry {
        PF_HashEntry *next;   // 下一个hash表的元素或则为空
        PF_HashEntry *prev;   // 前一个hash表的元素或则为空
        int          fd;      // 文件描述符
        PageNum      pageNum; // 页号
        int          slot;    // 当前页在内存缓冲池中的槽号
};


class PF_HashTable {
public:
    PF_HashTable (int numBuckets);                      // 构造函数
    ~PF_HashTable();                                    // 析构函数
    RC  Find     (int fd, PageNum pageNum, int &slot);  // 在hash表中找到文件描述符为 fd , 页号为pageNUm的缓冲块在缓冲池的插槽号
    RC  Insert   (int fd, PageNum pageNum, int slot);   // 往hash表中加入一个与新分配的缓冲块描述信息一致的表项
    RC  Delete   (int fd, PageNum pageNum);             //删除该表项

private:
    int Hash     (int fd, PageNum pageNum) const
    { return ((fd + pageNum) % numBuckets); }         // 散列函数
    int numBuckets;                                     // hash表桶数
    PF_HashEntry **hashTable;                           // hash表

};


#endif //DBMS_PF_HASHTABEL_H
