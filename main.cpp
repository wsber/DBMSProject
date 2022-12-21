#include <iostream>
#include "PF/pf.h"
#include "cmath"
#include <iostream>
#include <cstdio>
#include <string>
#include <unistd.h>
#include "redbase.h"
#include "RM/rm.h"


using namespace std ;





int main() {

    string diskSpace , dbName, tableName;
    const char *currentPosition;
    float memorySize = 0.0 ;
    int diskSpaceSize = 0 , pageSize = 0;
    /*cout<<"������Ҫ���������ռ��С memorySize (��λMB)\n";
    cin>> memorySize;
    cout<<"������Ҫ�����ڴ�ҳ��С (��λKB)\n";
    cin>>pageSize;
    PF_Manager pfm(ceil(memorySize*1024/pageSize),pageSize);*/
    /*cout<<"��������̿ռ�λ��\n";
    cin>>diskSpace;
    currentPosition = currentPosition + diskSpace + '/';
    cout<<"���������ݿ������\n";
    cin>>dbName;
    currentPosition = currentPosition + dbName + "/";
    cout<<"���������ݱ������\n";
    cin>>tableName;
    currentPosition = currentPosition + tableName + "/";
    cout<<"��ǰ�ļ�·��"<<currentPosition<<endl ;*/
    PF_Manager pfm;
    currentPosition = "D:\\ϵͳĬ��\\����\\���ݿ�Ӧ�ó������\\myredbase\\Data\\test.data";
    RM_Manager rmm(pfm);
    //�������ݱ�
    rmm.CreateFile(currentPosition,4000);
    RM_FileHandle relFH;
    //�򿪸����ݱ�
    rmm.OpenFile(currentPosition,relFH);
   /* string s = "001 database 40 6 001";
    char *recbuf = new char [s.size()];
    strcpy(recbuf,s.c_str());*/
    //���ñ��ڲ����¼����
    RID recRID;
    PageNum pageNum;
    SlotNum slotNum;
    //���ݼ�¼��RID��õ��ü�¼
    RM_Record rec ;
    char * pData;
    relFH.InsertRec("001 database 40 6 001",recRID);
    recRID.GetPageNum(pageNum);
    recRID.GetSlotNum(slotNum);
    cout<<pageNum<<"  "<<slotNum<<"����RID\n";

//    recRID.setPageNum(1);
//    recRID.setSlotNum(0);
    relFH.GetRec(recRID,rec);
    cout<<rec.GetData(pData)<<"�����ж��Ƿ�ɹ�\n";
    cout<<pData<<"����data\n";

//    relFH.InsertRec("002 dataStructure 40 6 003",recRID);
//    recRID.GetPageNum(pageNum);
//    recRID.GetSlotNum(slotNum);
//    cout<<pageNum<<"  "<<slotNum<<"����RID\n";
//    recRID.setPageNum(2);
//    recRID.setSlotNum(0);
//    relFH.GetRec(recRID,rec);
//    cout<<rec.GetData(pData)<<"�����ж��Ƿ�ɹ�\n";
//    cout<<pData<<"����data\n";
//
//    relFH.InsertRec("003 fundamentalsOfCompiling 40 6 002",recRID);
//    recRID.GetPageNum(pageNum);
//    recRID.GetSlotNum(slotNum);
//    cout<<pageNum<<"  "<<slotNum<<"����RID\n";
//    recRID.setPageNum(3);
//    recRID.setSlotNum(0);
//    relFH.GetRec(recRID,rec);
//    cout<<rec.GetData(pData)<<"�����ж��Ƿ�ɹ�\n";
//    cout<<pData<<"����data\n";
//
//
//
//    relFH.InsertRec("004 C 40 4.5 003",recRID);
//    recRID.GetPageNum(pageNum);
//    recRID.GetSlotNum(slotNum);
//    cout<<pageNum<<"  "<<slotNum<<"����RID\n";
//    recRID.setPageNum(4);
//    recRID.setSlotNum(0);
//    relFH.GetRec(recRID,rec);
//    cout<<rec.GetData(pData)<<"�����ж��Ƿ�ɹ�\n";
//    cout<<pData<<"����data\n";

    relFH.ForcePages(1);






    return 0;
}

////      PF_Manager * pfManager = new PF_Manager();
////      PF_FileHandle * pfFileHandle = new PF_FileHandle();
////      pfm.OpenFile("D:\\ϵͳĬ��\\����\\���ݿ�Ӧ�ó������\\myredbase\\test.data",*pfFileHandle);
////      pfm.CloseFile(*pfFileHandle);