//
// Created by hp on 2022/10/12.
//

#include "CLOCKPageReplacement.h"


CLOCKPageReplacement::CLOCKPageReplacement(int hashTableSize) : PF_pageReplacement(hashTableSize) {
    cout<<"CLOCK内存管理类构造完毕，接下来内存缓冲池的管理由该类负责";
    clockPoint = first;
}

RC CLOCKPageReplacement::execPageReplacement(int &slot) {

    RC rc;
    if(free != INVALID_SLOT){
        slot =free;
        free = bufTable[slot].next;
        cout<<"||空闲块链不空，将内存中信息读入slot块"<<slot<<"中";
        //将该槽放入使用链首
        // Link slot at the head of the used list
        if ((rc = LinkHead(slot)))
            return (rc);
    } else{
        //选择CLOCK页且不被定住
        cout<<"||空闲块链为空，执行页面淘汰算法CLOCK: ";

        int flag =  0;
        while (!flag){
            for (slot = clockPoint; slot != INVALID_SLOT  ; slot = bufTable[slot].next) {
                if (bufTable[slot].r)
                    bufTable[slot].r = 0 ;
                else{
                    flag = 1;
                    break;
                }
            }
            if(flag) break;
            for (slot = first; slot != clockPoint  ; slot = bufTable[slot].next) {
                if (bufTable[slot].r)
                    bufTable[slot].r = 0 ;
                else{
                    flag = 1;
                    break;
                }
            }
        }
        cout<<" || 淘汰的缓冲区槽号为 "<<slot<<"且对应的文件名描述符和页号分别为 : "<<bufTable[slot].fd<<"--"<<bufTable[slot].pageNum;
        if(bufTable[slot].next != INVALID_SLOT)
            clockPoint = bufTable[slot].next ;
        else clockPoint = first ;

        if (slot == INVALID_SLOT)
            return (PF_NOBUF);

        //如果当前页是脏的写回磁盘

        if(bufTable[slot].bDirty){
            if ((rc = WritePage(bufTable[slot].fd, bufTable[slot].pageNum,
                                bufTable[slot].pData)))
                return (rc);
            bufTable[slot].bDirty = FALSE;
            cout<<"||且该也页面为脏页要写回到磁盘\n";
        }
        // 把当前页从hash表和使用块链移除
        // Remove page from the hash table and slot from the used buffer list
        if ((rc = hashTable->Delete(bufTable[slot].fd, bufTable[slot].pageNum)))
            return (rc);

    }



    return 0;
}

RC CLOCKPageReplacement::GetPage(int fd, PageNum pageNum, char *ppBuffer, int bMultiplePins) {
    RC rc;
    int slot;

    if((rc = hashTable->Find(fd,pageNum,slot)) &&
       (rc != PF_HASHNOTFOUND))
        return rc;
    //页不在内存
    if(rc == PF_HASHNOTFOUND){

        cout<<"访问的页不在内存,当前CLOCK指针指向-->"<<clockPoint;
        if ((rc = this->execPageReplacement(slot)))
            return rc;

        bufTable[slot].r = 1 ;
        if((rc = ReadPage(fd , pageNum , bufTable[slot].pData))||
           (rc = hashTable->Insert(fd,pageNum,slot))||
           (rc = InitPageDesc(fd,pageNum,slot))){
            Unlink(slot);
            InsertFree(slot);
            return  (rc);
        }
    } else{
        cout<<"槽号为"<<slot<<"的页在内存,返回内存中该页的数据，将该页访问位 r 置1\n";
        cout<<"****************************************************************************************************"<<endl;
        bufTable[slot].r = 1;
            return (rc);
    }

    ppBuffer = bufTable[slot].pData;

    return  (0);
}

RC CLOCKPageReplacement::InsertFree(int slot) {
    bufTable[slot].next = free;
    free = slot;

    // Return ok
    return (0);
}

RC CLOCKPageReplacement::LinkHead(int slot) {
    // Set next and prev pointers of slot entry
    bufTable[slot].next = first;
    bufTable[slot].prev = INVALID_SLOT;

    // If list isn't empty, point old first back to slot
    if (first != INVALID_SLOT)
        bufTable[first].prev = slot;

    first = slot;

    // if list was empty, set last to slot
    if (last == INVALID_SLOT)
        last = first;

    // Return ok
    return (0);
}

RC CLOCKPageReplacement::Unlink(int slot) {
    // If slot is at head of list, set first to next element
    if (first == slot)
        first = bufTable[slot].next;

    // If slot is at end of list, set last to previous element
    if (last == slot)
        last = bufTable[slot].prev;

    // If slot not at end of list, point next back to previous
    if (bufTable[slot].next != INVALID_SLOT)
        bufTable[bufTable[slot].next].prev = bufTable[slot].prev;

    // If slot not at head of list, point prev forward to next
    if (bufTable[slot].prev != INVALID_SLOT)
        bufTable[bufTable[slot].prev].next = bufTable[slot].next;

    // Set next and prev pointers of slot entry
    bufTable[slot].prev = bufTable[slot].next = INVALID_SLOT;

    // Return ok
    return (0);
}
