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
//      pfm.OpenFile("D:\\ϵͳĬ��\\����\\���ݿ�Ӧ�ó������\\myredbase\\test.data",*pfFileHandle);
//      pfm.CloseFile(*pfFileHandle);
      cout<<"������main"<<endl;
      //��������
      ixm.CreateIndex("D:\\ϵͳĬ��\\����\\���ݿ�Ӧ�ó������\\myredbase\\test.txt",1,INT ,4);

      //�������ݱ�
      rmm.CreateFile("D:\\ϵͳĬ��\\����\\���ݿ�Ӧ�ó������\\myredbase\\test.txt",40);
      RM_FileHandle relFH;
      //�򿪸����ݱ�
      rmm.OpenFile("D:\\ϵͳĬ��\\����\\���ݿ�Ӧ�ó������\\myredbase\\test.txt",relFH);


      string s = "sdadda";
      char *recbuf = new char [s.size()];
      strcpy(recbuf,s.c_str());

      //���ñ��ڲ����¼����
      RID recRID;
      for(int i = 0 ; i < 10 ;i++){
          relFH.InsertRec(recbuf,recRID);
          PageNum pageNum;
          SlotNum slotNum;
          recRID.GetPageNum(pageNum);
          recRID.GetSlotNum(slotNum);
          cout<<pageNum<<"  "<<slotNum<<"����RID\n";
      }


      //���ݼ�¼��RID��õ��ü�¼
      RM_Record rec ;
      relFH.GetRec(recRID,rec);
      char * pData;
      cout<<rec.GetData(pData)<<"����data\n";
      cout<<pData<<"����data\n";
      cout<<"\n���ļ����"<<endl;



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
//    cout<<endl<<pData<<"���е�������";



//  cout<<"���濪ʼgoto\n";
//  goto lable1;


  lable1:
//    cout<<"goto��������";







    return 0;
}
