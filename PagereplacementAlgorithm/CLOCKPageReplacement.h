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
    RC execPageReplacement(int &slot) override ;                                      //ִ��CLOCK�û��㷨
    RC GetPage(int fd, PageNum pageNum, char *ppBuffer, int bMultiplePins) override;
    RC  InsertFree   (int slot) override;                                            // �ڿ��п���ͷ����ۺ�Ϊslot�Ŀ��п飨ʵ��Ϊ���PF_BufPageDesc�ṹ���ڲ�ָ��ֵ��
    RC  LinkHead     (int slot) override;                                            // ��ʹ�ÿ���ͷ����ۺ�Ϊslot�Ļ����
    RC  Unlink       (int slot) override;

private:
    int clockPoint;

};


#endif //DBMS_CLOCKPAGEREPLACEMENT_H
