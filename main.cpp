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

/*    //测试windows fopen文件读写
    FILE * p = fopen("D:\\系统默认\\桌面\\数据库应用程序设计\\myredbase\\test.data","w");
    fputs("Hello",p);*/
/*    //测试linux open文件读写
    int fd, size;
    char s[] = "Linux Programmer!\n", buffer[80];
    fd = open("D:\\\\系统默认\\\\桌面\\\\数据库应用程序设计\\\\myredbase\\\\Data\\\\test.data", O_WRONLY|O_CREAT);
    cout<<fd<<endl;
    write(fd, s, sizeof(s));
    int fd2;
    fd2 = open("D:\\\\系统默认\\\\桌面\\\\数据库应用程序设计\\\\myredbase\\\\Data\\\\test2.data", O_WRONLY|O_CREAT);
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
    cout<<"请输入要申请的主存空间大小 memorySize (单位MB)\n";
    cin>> memorySize;
    cout<<"请输入要申请内存页大小 (单位KB)\n";
    cin>>pageSize;
    PF_Manager pfm(ceil(memorySize*1024/pageSize),pageSize);
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
//    PF_Manager pfm;
    currentPosition = "D:\\系统默认\\桌面\\数据库应用程序设计\\myredbase\\Data\\course.data";
    RM_Manager rmm(pfm);
    //创建数据表
    rmm.CreateFile(currentPosition,50);
    RM_FileHandle relFH;
    //打开该数据表
    rmm.OpenFile(currentPosition,relFH);

    //往该表内插入记录数据
    RID recRID(1,0);
    PageNum pageNum;
    SlotNum slotNum;
    //根据记录的RID查得到该记录
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
////        cout<<"该记录插入的页号："<<pageNum<<"  该记录插入该页的槽号"<<slotNum<<"这是RID\n";
//    }

//    recRID.setPageNum(1);
//    recRID.setSlotNum(0);
//    relFH.GetRec(recRID,rec);
//    cout<<rec.GetData(pData)<<"这是判断是否成功\n";
//    cout<<pData<<"这是data\n";
    rmm.CloseFile(relFH);



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







    return 0;
}

////      PF_Manager * pfManager = new PF_Manager();
////      PF_FileHandle * pfFileHandle = new PF_FileHandle();
////      pfm.OpenFile("D:\\系统默认\\桌面\\数据库应用程序设计\\myredbase\\test.data",*pfFileHandle);
////      pfm.CloseFile(*pfFileHandle);

//string s = "001 database 40 6 001";
//char *recbuf = new char [s.size()];
//strcpy(recbuf,s.c_str());

//IX_Manager ixm(pfm);
//ixm.CreateIndex("D:\\系统默认\\桌面\\数据库应用程序设计\\myredbase\\Data\\course.data",1,STRING,10);
//IX_IndexHandle ixh;
//ixm.OpenIndex("D:\\系统默认\\桌面\\数据库应用程序设计\\myredbase\\Data\\course.data",1,ixh);
//cout<<ixh.InsertEntry((void*)"database",*(new RID(1,0)));
//cout<<ixh.InsertEntry((void*)"c",*(new RID(1,0)));
//cout<<ixh.InsertEntry((void*)"os",*(new RID(1,1)));
//ixh.InsertEntry((void*)"sa",*(new RID(1,2)));
//ixh.InsertEntry((void*)"ssad",*(new RID(1,3)));
//ixh.PrintIndex();
