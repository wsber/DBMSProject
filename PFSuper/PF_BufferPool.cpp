//
// Created by hp on 2022/10/12.
//

#include "PF_BufferPool.h"



PF_BufferPool::~PF_BufferPool() {
    for (int i = 0; i < this->numPages; i++)
        delete [] bufTable[i].pData;

    delete [] bufTable;
}

PF_BufferPool::PF_BufferPool(int numPages, PF_pageReplacement *pfPageReplacement) {

    this->numPages = numPages;
    pageSize = PF_PAGE_SIZE +sizeof(PF_PageHdr);
    bufTable = new PF_BufPageDesc[numPages];
    for (int i = 0; i < numPages; i++) {
        if ((bufTable[i].pData = new char[pageSize]) == NULL) {
            cerr << "Not enough memory for buffer\n";
            exit(1);
        }
        memset ((void *)bufTable[i].pData, 0, pageSize);
        bufTable[i].prev = i - 1;
        bufTable[i].next = i + 1;
        bufTable[i].pinCount = 0 ;
        bufTable[i].r = 0 ;
    }
    bufTable[0].prev = bufTable[numPages - 1].next = INVALID_SLOT;
    this->pfPageReplacement = pfPageReplacement;
    this->pfPageReplacement->bufTable = bufTable;
    cout<<"PF_BufferPool¹¹ÔìÍê±Ï"<<endl;

}

RC PF_BufferPool::GetPage(int fd, PageNum pageNum, char *ppBuffer, int bMultiplePins) {

    return  this->pfPageReplacement->GetPage(fd,pageNum,ppBuffer,bMultiplePins);

}

RC PF_BufferPool::MarkDirty(int fd, PageNum pageNum) {
    return 0;
}

RC PF_BufferPool::UnpinPage(int fd, PageNum pageNum) {
    return 0;
}

RC PF_BufferPool::ForcePages(int fd, PageNum pageNum) {
    return 0;
}
