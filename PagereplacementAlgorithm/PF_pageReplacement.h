//
// Created by hp on 2022/10/11.
//

#ifndef DBMS_PF_PAGEREPLACEMENT_H
#define DBMS_PF_PAGEREPLACEMENT_H
//#include "../PFSuper/pfSuper.h"
#include "../PFSuper/PF_internal.h"
#include "../PFSuper/PF_HashTable.h"
#include <iostream>
#define INVALID_SLOT  (-1)

struct PF_BufPageDesc {
    char       *pData;      // 块（页）数据内容
    int        next;        // 下一个块的块号
    int        prev;        // 前一个块的块号
    int        bDirty;      // 脏页的标志位
    short int  pinCount;    // 引用计数
    PageNum    pageNum;     // 页号
    int        fd;          // 文件描述符
    int         r;          // 读标志位--CLOCK算法
};

class PF_pageReplacement
{

public:
    PF_pageReplacement(int hashTableSize = 1000);
    ~PF_pageReplacement(){};
    virtual RC execPageReplacement(int& slot) ;   // 执行页面淘汰算法
    virtual RC GetPage(int fd, PageNum pageNum, char *ppBuffer,int bMultiplePins);
    virtual RC  InsertFree   (int slot);                                            // 在空闲块链头插入槽号为slot的空闲块（实质为变更PF_BufPageDesc结构体内部指针值）
    virtual RC  LinkHead     (int slot);                                            // 在使用块链头插入槽号为slot的缓冲块
    virtual RC  Unlink       (int slot);                                            // 释放槽号为slot的被使用的缓冲块，将其加入空闲块链
    RC  InitPageDesc (int fd, PageNum pageNum, int slot);// 得到一个槽号slot去使用
    RC  ReadPage     (int fd, PageNum pageNum, char *dest);                         //将文件描述符为fd ,页号为pageNum的页的内容读入dest
    RC  WritePage    (int fd, PageNum pageNum, char *source);                       //将source中的内容写入文件描述符为fd ,页号为pageNum的页
    PF_HashTable * hashTable;                    //hash表：用于快速查询内存中是否存在文件描述符为fd ,页号为pageNum的缓冲块（页）
    PF_BufPageDesc* bufTable;                    //内存缓冲池

protected:
    int first;                                   //使用块链中第一个槽号
    int last;                                    //使用块链中最后一个槽号
    int free;                                    //空闲块连中第一个槽号
};


#endif //DBMS_PF_PAGEREPLACEMENT_H
