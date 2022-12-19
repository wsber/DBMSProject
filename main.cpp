#include <iostream>
#include "PF/pf_hashtable.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include "redbase.h"
#include "RM/rm.h"


using namespace std ;


PF_Manager pfm;
RM_Manager rmm(pfm);

int main() {

      PF_Manager * pfManager = new PF_Manager();
      PF_FileHandle * pfFileHandle = new PF_FileHandle();
      cout<<"这里是main"<<endl;
      pfManager->OpenFile("D:\\系统默认\\桌面\\数据库应用程序设计\\myredbase\\test.data",*pfFileHandle);
      pfManager->CloseFile(*pfFileHandle);
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
