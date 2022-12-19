//
// Created by hp on 2022/10/12.
//

#ifndef DBMS_CLOCKPAGEREPLACEMENT_H
#define DBMS_CLOCKPAGEREPLACEMENT_H
#include "PF_pageReplacement.h"

class CLOCKPageReplacement : public PF_pageReplacement {
public:
    CLOCKPageReplacement(int hashTableSize);
    ~CLOCKPageReplacement(){}
    RC execPageReplacement(int &slot) override ;                                      //执行CLOCK置换算法
    RC GetPage(int fd, PageNum pageNum, char *ppBuffer, int bMultiplePins) override;
    RC  InsertFree   (int slot) override;                                            // 在空闲块链头插入槽号为slot的空闲块（实质为变更PF_BufPageDesc结构体内部指针值）
    RC  LinkHead     (int slot) override;                                            // 在使用块链头插入槽号为slot的缓冲块
    RC  Unlink       (int slot) override;

private:
    int clockPoint;

};


#endif //DBMS_CLOCKPAGEREPLACEMENT_H
