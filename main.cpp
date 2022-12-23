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

    string diskSpace , dbName, tableName ,path;
    char *currentPosition = nullptr;
    float memorySize = 0.0 ;
    int diskSpaceSize = 0 , pageSize = 0;
    cout<<"������Ҫ���������ռ��С memorySize (��λMB)\n";
    cin>> memorySize;
    cout<<"�������ڴ�ҳ���С (��λKB�������������Сһ��)\n";
    cin>>pageSize;
    PF_Manager pfm(ceil(memorySize*1024/pageSize),pageSize);
   /* cout<<"��������̿ռ�λ��\n";
    cin>>diskSpace;
    path = path + diskSpace + '/';
    cout<<"���������ݿ������\n";
    cin>>dbName;
    path = path + dbName + "/";
    path += "Data/";
    cout<<"���������ݱ������\n";
    cin>>tableName;
    path = path + tableName + ".data";*/
    //    cout<<"��ǰ�ļ�·��"<<path<<endl ;

    path = "D:/ϵͳĬ��/����/���ݿ�Ӧ�ó������/myredbase/Data/course.data";


    currentPosition= const_cast<char *>(path.c_str());
    cout<<currentPosition<<endl;

    RM_Manager rmm(pfm);
    //�������ݱ�
//    rmm.CreateFile(currentPosition,50);
    RM_FileHandle relFH;
    //�򿪸����ݱ�
    rmm.OpenFile(currentPosition,relFH);

    //���ñ��ڲ����¼����
    RID recRID(1,0);
    PageNum pageNum;
    SlotNum slotNum;
    //���ݼ�¼��RID��õ��ü�¼


//    for(int i =0 ; i < 10000 ;i++){
//        string tmp = to_string(i+1)+ " database 40 6 001";
//        relFH.InsertRec(const_cast<char *>(tmp.c_str()),recRID);
//        recRID.GetPageNum(pageNum);
//        recRID.GetSlotNum(slotNum);
//        cout<<"�����Ǹü�¼��RID�� "<<"�ü�¼�����ҳ�ţ�"<<pageNum<<"  �ü�¼�����ҳ�Ĳۺ�"<<slotNum<<"\n";
//    }

    for(int i = 1 ; i <= 124  ;i++){
        recRID.setPageNum(i);
        for(int j = 0 ; j <= 80 ; j++){
               RM_Record rec ;
               char * pData;
               recRID.setSlotNum(j);
               relFH.GetRec(recRID,rec);
               rec.GetData(pData);
               cout<<"���ǲ�����еü�¼: "<<pData<<"\n";
        }
    }
    rmm.CloseFile(relFH);




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



//    relFH.InsertRec("001 database 40 6 001",recRID);
//    relFH.InsertRec("002 c 40 6 001",recRID);
//    relFH.InsertRec("003 os 40 6 001",recRID);
//    relFH.InsertRec("004 sa 40 6 001",recRID);
//    relFH.InsertRec("005 ssad 40 6 001",recRID);