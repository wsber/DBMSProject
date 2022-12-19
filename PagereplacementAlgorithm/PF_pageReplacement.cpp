//
// Created by hp on 2022/10/11.
//

#include "PF_pageReplacement.h"



PF_pageReplacement::PF_pageReplacement(int hashTableSize)
{
    this->hashTable = new PF_HashTable(hashTableSize);
    this->bufTable = 0 ;
    this->free = 0 ;
    first = last = INVALID_SLOT;
}

RC PF_pageReplacement::GetPage(int fd, PageNum pageNum, char *ppBuffer, int bMultiplePins) {
    return  0 ;
}

RC PF_pageReplacement::execPageReplacement(int& slot) {
    return 0;
}

RC PF_pageReplacement::Unlink(int slot) {
    return 0;
}

RC PF_pageReplacement::InsertFree(int slot) {
    return 0;
}

RC PF_pageReplacement::LinkHead(int slot) {
    return 0;
}



RC PF_pageReplacement::ReadPage(int fd, PageNum pageNum, char *dest) {
    string  s = "|| 将文件描述符为" + to_string(fd)  +"和页号为"+ to_string(pageNum) + "的外存物理块读入dest";
    dest = (char*)s.data();
    cout<<s<<endl;
    cout<<"****************************************************************************************************"<<endl;
    return 0;
}

RC PF_pageReplacement::WritePage(int fd, PageNum pageNum, char *source) {
    string  s = "|| 将source中的数据写回文件描述符为" + to_string(fd)  +"和页号"+ to_string(pageNum) + "对应的外存物理块中";
    source = (char*)s.data();
    cout<<s<<endl;
    cout<<"****************************************************************************************************"<<endl;
    return 0;
}

RC PF_pageReplacement::InitPageDesc(int fd, PageNum pageNum, int slot) {
    // set the slot to refer to a newly-pinned page
    bufTable[slot].fd       = fd;
    bufTable[slot].pageNum  = pageNum;
    bufTable[slot].bDirty   = FALSE;
    bufTable[slot].pinCount = 0;

    // Return ok
    return (0);
}

