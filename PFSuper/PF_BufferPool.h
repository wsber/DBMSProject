//
// Created by hp on 2022/10/12.
//

#ifndef DBMS_PF_BUFFERPOOL_H
#define DBMS_PF_BUFFERPOOL_H

#include "pf.h"
#include "../PagereplacementAlgorithm/PF_pageReplacement.h"

struct PF_BufPageDesc ;

class PF_BufferPool{
public:

    PF_BufferPool(int numPages,PF_pageReplacement *pfPageReplacement);   //构造函数，将页面置换管理类指针传递到PF_BufferPool中
    ~PF_BufferPool();                         // 析构函数
    RC  GetPage      (int fd, PageNum pageNum, char *ppBuffer,int bMultiplePins = TRUE); // 在缓冲池中找到满足文件描述符为fd ,页面号为pageNum的缓冲块，并使ppBuffer指向该缓冲块保存的数据集
    RC  MarkDirty    (int fd, PageNum pageNum);  // 将一页置为脏页
    RC  UnpinPage    (int fd, PageNum pageNum);  // 将某页从内存取消固定
    RC ForcePages    (int fd, PageNum pageNum);  // 强制性写回一页到磁盘，但此页仍在内存保留
private:
    PF_BufPageDesc *bufTable;                     // 结构体指针数组，存储一组从内存申请的缓冲块
    PF_pageReplacement *pfPageReplacement;        // 页面（缓冲块）置换管理类指针，将缓冲区内缓冲块的读写和换入换出等任务委托给该类指针，通过多态实现对同一缓冲区选择使用多种页面置换算法
    int            numPages;                      // 在缓冲区中缓冲块个数
    int            pageSize;                      // 缓冲去中缓冲块大小
    int            first;                         // used Block 链中第一块的槽号
    int            last;                          // used Block 链中最后一块的槽号
    int            free;                          // 空闲链中第一块的槽号

};


#endif //DBMS_PF_BUFFERPOOL_H
