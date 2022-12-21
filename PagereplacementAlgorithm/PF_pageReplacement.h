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
    char       *pData;      // �飨ҳ����������
    int        next;        // ��һ����Ŀ��
    int        prev;        // ǰһ����Ŀ��
    int        bDirty;      // ��ҳ�ı�־λ
    short int  pinCount;    // ���ü���
    PageNum    pageNum;     // ҳ��
    int        fd;          // �ļ�������
    int         r;          // ����־λ--CLOCK�㷨
};

class PF_pageReplacement
{

public:
    PF_pageReplacement(int hashTableSize = 1000);
    ~PF_pageReplacement(){};
    virtual RC execPageReplacement(int& slot) ;   // ִ��ҳ����̭�㷨
    virtual RC GetPage(int fd, PageNum pageNum, char *ppBuffer,int bMultiplePins);
    virtual RC  InsertFree   (int slot);                                            // �ڿ��п���ͷ����ۺ�Ϊslot�Ŀ��п飨ʵ��Ϊ���PF_BufPageDesc�ṹ���ڲ�ָ��ֵ��
    virtual RC  LinkHead     (int slot);                                            // ��ʹ�ÿ���ͷ����ۺ�Ϊslot�Ļ����
    virtual RC  Unlink       (int slot);                                            // �ͷŲۺ�Ϊslot�ı�ʹ�õĻ���飬���������п���
    RC  InitPageDesc (int fd, PageNum pageNum, int slot);// �õ�һ���ۺ�slotȥʹ��
    RC  ReadPage     (int fd, PageNum pageNum, char *dest);                         //���ļ�������Ϊfd ,ҳ��ΪpageNum��ҳ�����ݶ���dest
    RC  WritePage    (int fd, PageNum pageNum, char *source);                       //��source�е�����д���ļ�������Ϊfd ,ҳ��ΪpageNum��ҳ
    PF_HashTable * hashTable;                    //hash�����ڿ��ٲ�ѯ�ڴ����Ƿ�����ļ�������Ϊfd ,ҳ��ΪpageNum�Ļ���飨ҳ��
    PF_BufPageDesc* bufTable;                    //�ڴ滺���

protected:
    int first;                                   //ʹ�ÿ����е�һ���ۺ�
    int last;                                    //ʹ�ÿ��������һ���ۺ�
    int free;                                    //���п����е�һ���ۺ�
};


#endif //DBMS_PF_PAGEREPLACEMENT_H
