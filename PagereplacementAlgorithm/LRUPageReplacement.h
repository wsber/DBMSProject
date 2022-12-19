//
// Created by hp on 2022/10/11.
//

#ifndef DBMS_LRUPAGEREPLACEMENT_H
#define DBMS_LRUPAGEREPLACEMENT_H
#include "PF_pageReplacement.h"


class LRUPageReplacement : public PF_pageReplacement  {

public:
    LRUPageReplacement(int hahTableSize);
    ~LRUPageReplacement(){}
    RC execPageReplacement(int &slot) override ;
    RC GetPage(int fd, PageNum pageNum, char *ppBuffer, int bMultiplePins) override;
    RC  InsertFree   (int slot) override;                                            // 在空闲块链头插入槽号为slot的空闲块（实质为变更PF_BufPageDesc结构体内部指针值）
    RC  LinkHead     (int slot) override;                                            // 在使用块链头插入槽号为slot的缓冲块
    RC  Unlink       (int slot) override;

private:


};


#endif //DBMS_LRUPAGEREPLACEMENT_H
