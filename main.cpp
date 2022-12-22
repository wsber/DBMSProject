#include <iostream>
#include "PF/pf.h"
#include "cmath"
#include <iostream>
#include <cstdio>
#include <string>
#include <unistd.h>
#include "redbase.h"
#include "RM/rm.h"
#include <fcntl.h>
#include "IX/ix.h"


using namespace std ;





int main() {

/*    //����windows fopen�ļ���д
    FILE * p = fopen("D:\\ϵͳĬ��\\����\\���ݿ�Ӧ�ó������\\myredbase\\test.data","w");
    fputs("Hello",p);*/
/*    //����linux open�ļ���д
    int fd, size;
    char s[] = "Linux Programmer!\n", buffer[80];
    fd = open("D:\\\\ϵͳĬ��\\\\����\\\\���ݿ�Ӧ�ó������\\\\myredbase\\\\Data\\\\test.data", O_WRONLY|O_CREAT);
    cout<<fd<<endl;
    write(fd, s, sizeof(s));
    int fd2;
    fd2 = open("D:\\\\ϵͳĬ��\\\\����\\\\���ݿ�Ӧ�ó������\\\\myredbase\\\\Data\\\\test2.data", O_WRONLY|O_CREAT);
    write(fd2, "wsssdas", sizeof(s));
    cout<<fd2;
    close(fd);
    close(fd2);*/


//        fd = open("data/test.data", O_RDONLY);
//        size = read(fd, buffer, sizeof(buffer));
//        close(fd);
//        printf("%s", buffer);

    string diskSpace , dbName, tableName;
    const char *currentPosition;
    float memorySize = 0.0 ;
    int diskSpaceSize = 0 , pageSize = 0;
    cout<<"������Ҫ���������ռ��С memorySize (��λMB)\n";
    cin>> memorySize;
    cout<<"������Ҫ�����ڴ�ҳ��С (��λKB)\n";
    cin>>pageSize;
    PF_Manager pfm(ceil(memorySize*1024/pageSize),pageSize);
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
//    PF_Manager pfm;
    currentPosition = "D:\\ϵͳĬ��\\����\\���ݿ�Ӧ�ó������\\myredbase\\Data\\course.data";
    RM_Manager rmm(pfm);
    //�������ݱ�
    rmm.CreateFile(currentPosition,50);
    RM_FileHandle relFH;
    //�򿪸����ݱ�
    rmm.OpenFile(currentPosition,relFH);

    //���ñ��ڲ����¼����
    RID recRID(1,0);
    PageNum pageNum;
    SlotNum slotNum;
    //���ݼ�¼��RID��õ��ü�¼
    RM_Record rec ;
    char * pData;
    relFH.InsertRec("001 database 40 6 001",recRID);
    relFH.InsertRec("002 c 40 6 001",recRID);
    relFH.InsertRec("003 os 40 6 001",recRID);
    relFH.InsertRec("004 sa 40 6 001",recRID);
    relFH.InsertRec("005 ssad 40 6 001",recRID);

//    for(int i =0 ; i < 10000 ;i++){
//        relFH.InsertRec("001 database 40 6 001",recRID);
////        recRID.GetPageNum(pageNum);
////        recRID.GetSlotNum(slotNum);
////        cout<<"�ü�¼�����ҳ�ţ�"<<pageNum<<"  �ü�¼�����ҳ�Ĳۺ�"<<slotNum<<"����RID\n";
//    }

//    recRID.setPageNum(1);
//    recRID.setSlotNum(0);
//    relFH.GetRec(recRID,rec);
//    cout<<rec.GetData(pData)<<"�����ж��Ƿ�ɹ�\n";
//    cout<<pData<<"����data\n";
    rmm.CloseFile(relFH);



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







    return 0;
}

////      PF_Manager * pfManager = new PF_Manager();
////      PF_FileHandle * pfFileHandle = new PF_FileHandle();
////      pfm.OpenFile("D:\\ϵͳĬ��\\����\\���ݿ�Ӧ�ó������\\myredbase\\test.data",*pfFileHandle);
////      pfm.CloseFile(*pfFileHandle);

//string s = "001 database 40 6 001";
//char *recbuf = new char [s.size()];
//strcpy(recbuf,s.c_str());

//IX_Manager ixm(pfm);
//ixm.CreateIndex("D:\\ϵͳĬ��\\����\\���ݿ�Ӧ�ó������\\myredbase\\Data\\course.data",1,STRING,10);
//IX_IndexHandle ixh;
//ixm.OpenIndex("D:\\ϵͳĬ��\\����\\���ݿ�Ӧ�ó������\\myredbase\\Data\\course.data",1,ixh);
//cout<<ixh.InsertEntry((void*)"database",*(new RID(1,0)));
//cout<<ixh.InsertEntry((void*)"c",*(new RID(1,0)));
//cout<<ixh.InsertEntry((void*)"os",*(new RID(1,1)));
//ixh.InsertEntry((void*)"sa",*(new RID(1,2)));
//ixh.InsertEntry((void*)"ssad",*(new RID(1,3)));
//ixh.PrintIndex();
