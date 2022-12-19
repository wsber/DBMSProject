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
    RC  InsertFree   (int slot) override;                                            // �ڿ��п���ͷ����ۺ�Ϊslot�Ŀ��п飨ʵ��Ϊ���PF_BufPageDesc�ṹ���ڲ�ָ��ֵ��
    RC  LinkHead     (int slot) override;                                            // ��ʹ�ÿ���ͷ����ۺ�Ϊslot�Ļ����
    RC  Unlink       (int slot) override;

private:


};


#endif //DBMS_LRUPAGEREPLACEMENT_H
