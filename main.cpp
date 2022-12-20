#include <iostream>
#include "PF/pf_hashtable.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include "redbase.h"
#include "RM/rm.h"
#include "IX/ix.h"


using namespace std ;


PF_Manager pfm;
RM_Manager rmm(pfm);
IX_Manager ixm(pfm);

int main() {

//      PF_Manager * pfManager = new PF_Manager();
//      PF_FileHandle * pfFileHandle = new PF_FileHandle();
//      pfm.OpenFile("D:\\系统默认\\桌面\\数据库应用程序设计\\myredbase\\test.data",*pfFileHandle);
//      pfm.CloseFile(*pfFileHandle);
      cout<<"这里是main"<<endl;
      //创建索引
      ixm.CreateIndex("D:\\系统默认\\桌面\\数据库应用程序设计\\myredbase\\test.txt",1,INT ,4);

      //创建数据表
      rmm.CreateFile("D:\\系统默认\\桌面\\数据库应用程序设计\\myredbase\\test.txt",40);
      RM_FileHandle relFH;
      //打开该数据表
      rmm.OpenFile("D:\\系统默认\\桌面\\数据库应用程序设计\\myredbase\\test.txt",relFH);


      string s = "sdadda";
      char *recbuf = new char [s.size()];
      strcpy(recbuf,s.c_str());

      //往该表内插入记录数据
      RID recRID;
      for(int i = 0 ; i < 10 ;i++){
          relFH.InsertRec(recbuf,recRID);
          PageNum pageNum;
          SlotNum slotNum;
          recRID.GetPageNum(pageNum);
          recRID.GetSlotNum(slotNum);
          cout<<pageNum<<"  "<<slotNum<<"这是RID\n";
      }


      //根据记录的RID查得到该记录
      RM_Record rec ;
      relFH.GetRec(recRID,rec);
      char * pData;
      cout<<rec.GetData(pData)<<"这是data\n";
      cout<<pData<<"这是data\n";
      cout<<"\n打开文件完毕"<<endl;



//    PF_pageReplacement *pageReplacement;
//    char* pData;
////    pageReplacement = new LRUPageReplacement(100);
////    PF_BufferPool*  pfBufferPool = new PF_BufferPool(5,pageReplacement);
////    pfBufferPool->GetPage(0,1, pData , 1);
////    pfBufferPool->GetPage(0,2,pData,1);
////    pfBufferPool->GetPage(0,3,pData,1);
////    pfBufferPool->GetPage(0,4, pData , 1);
////    pfBufferPool->GetPage(1,4, pData , 1);
////    pfBufferPool->GetPage(0,1,pData,1);
////    pfBufferPool->GetPage(1,2, pData , 1);
//
//
//
//    pageReplacement = new CLOCKPageReplacement(100);
//    PF_BufferPool*  pfBufferPool = new PF_BufferPool(5,pageReplacement);
//    pfBufferPool->GetPage(0,1, pData , 1);
//    pfBufferPool->GetPage(0,1, pData,1);
//    pfBufferPool->GetPage(0,2, pData,1);
//    pfBufferPool->GetPage(0,3, pData,1);
//    pfBufferPool->GetPage(0,4, pData , 1);
//    pfBufferPool->GetPage(1,4, pData , 1);
//    pfBufferPool->GetPage(1,2, pData , 1);
//    pfBufferPool->GetPage(0,4, pData , 1);
//    pfBufferPool->GetPage(2,5, pData , 1);
//
//    cout<<endl<<pData<<"运行到这里了";



//  cout<<"下面开始goto\n";
//  goto lable1;


  lable1:
//    cout<<"goto到了这里";







    return 0;
}
