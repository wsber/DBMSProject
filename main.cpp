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
    /*cout<<"请输入要申请的主存空间大小 memorySize (单位MB)\n";
    cin>> memorySize;
    cout<<"请输入要申请内存页大小 (单位KB)\n";
    cin>>pageSize;
    PF_Manager pfm(ceil(memorySize*1024/pageSize),pageSize);*/
    /*cout<<"请输入磁盘空间位置\n";
    cin>>diskSpace;
    currentPosition = currentPosition + diskSpace + '/';
    cout<<"请输入数据库的名字\n";
    cin>>dbName;
    currentPosition = currentPosition + dbName + "/";
    cout<<"请输入数据表的名字\n";
    cin>>tableName;
    currentPosition = currentPosition + tableName + "/";
    cout<<"当前文件路径"<<currentPosition<<endl ;*/
    PF_Manager pfm;
    currentPosition = "D:\\系统默认\\桌面\\数据库应用程序设计\\myredbase\\Data\\test.data";
    RM_Manager rmm(pfm);
    //创建数据表
    rmm.CreateFile(currentPosition,4000);
    RM_FileHandle relFH;
    //打开该数据表
    rmm.OpenFile(currentPosition,relFH);
   /* string s = "001 database 40 6 001";
    char *recbuf = new char [s.size()];
    strcpy(recbuf,s.c_str());*/
    //往该表内插入记录数据
    RID recRID;
    PageNum pageNum;
    SlotNum slotNum;
    //根据记录的RID查得到该记录
    RM_Record rec ;
    char * pData;
    relFH.InsertRec("001 database 40 6 001",recRID);
    recRID.GetPageNum(pageNum);
    recRID.GetSlotNum(slotNum);
    cout<<pageNum<<"  "<<slotNum<<"这是RID\n";

//    recRID.setPageNum(1);
//    recRID.setSlotNum(0);
    relFH.GetRec(recRID,rec);
    cout<<rec.GetData(pData)<<"这是判断是否成功\n";
    cout<<pData<<"这是data\n";

//    relFH.InsertRec("002 dataStructure 40 6 003",recRID);
//    recRID.GetPageNum(pageNum);
//    recRID.GetSlotNum(slotNum);
//    cout<<pageNum<<"  "<<slotNum<<"这是RID\n";
//    recRID.setPageNum(2);
//    recRID.setSlotNum(0);
//    relFH.GetRec(recRID,rec);
//    cout<<rec.GetData(pData)<<"这是判断是否成功\n";
//    cout<<pData<<"这是data\n";
//
//    relFH.InsertRec("003 fundamentalsOfCompiling 40 6 002",recRID);
//    recRID.GetPageNum(pageNum);
//    recRID.GetSlotNum(slotNum);
//    cout<<pageNum<<"  "<<slotNum<<"这是RID\n";
//    recRID.setPageNum(3);
//    recRID.setSlotNum(0);
//    relFH.GetRec(recRID,rec);
//    cout<<rec.GetData(pData)<<"这是判断是否成功\n";
//    cout<<pData<<"这是data\n";
//
//
//
//    relFH.InsertRec("004 C 40 4.5 003",recRID);
//    recRID.GetPageNum(pageNum);
//    recRID.GetSlotNum(slotNum);
//    cout<<pageNum<<"  "<<slotNum<<"这是RID\n";
//    recRID.setPageNum(4);
//    recRID.setSlotNum(0);
//    relFH.GetRec(recRID,rec);
//    cout<<rec.GetData(pData)<<"这是判断是否成功\n";
//    cout<<pData<<"这是data\n";

    relFH.ForcePages(1);






    return 0;
}

////      PF_Manager * pfManager = new PF_Manager();
////      PF_FileHandle * pfFileHandle = new PF_FileHandle();
////      pfm.OpenFile("D:\\系统默认\\桌面\\数据库应用程序设计\\myredbase\\test.data",*pfFileHandle);
////      pfm.CloseFile(*pfFileHandle);