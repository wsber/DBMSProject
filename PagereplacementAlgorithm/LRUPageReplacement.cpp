//
// Created by hp on 2022/10/11.
//

#include "LRUPageReplacement.h"

LRUPageReplacement::LRUPageReplacement(int hashTableSize)
        : PF_pageReplacement(hashTableSize) {
    cout<<"LRU内存管理类构造完毕，接下来将由该类负责对PF_BufferPool的管理";
}

RC LRUPageReplacement::GetPage(int fd, PageNum pageNum, char *ppBuffer, int bMultiplePins) {

    RC rc;
    int slot;

    if((rc = hashTable->Find(fd,pageNum,slot)) &&
            (rc != PF_HASHNOTFOUND))
        return rc;
    //页不在内存
    if(rc == PF_HASHNOTFOUND){
        cout<<"访问的页不在内存";
        if ((rc = this->execPageReplacement(slot)))
            return rc;

        if((rc = ReadPage(fd , pageNum , bufTable[slot].pData))||
                (rc = hashTable->Insert(fd,pageNum,slot))||
                (rc = InitPageDesc(fd,pageNum,slot))){
            Unlink(slot);
            InsertFree(slot);
            return  (rc);
        }

    } else{

        cout<<"访问页在内存,将该页放入到使用链首\n";
        cout<<"****************************************************************************************************"<<endl;
//        if (!bMultiplePins && bufTable[slot].pinCount > 0)
//            return (PF_PAGEPINNED);
        //每使用一次将这个页定住
//        bufTable[slot].pinCount++;
//        cout<<bufTable[slot].pinCount<<endl;
        if((rc = Unlink(slot))|| (rc = LinkHead(slot)))
            return (rc);

    }
//    cout<<bufTable[slot].pData<<"我到了getPage最后面啦";
    ppBuffer = bufTable[slot].pData;

    return  (0);


}

RC LRUPageReplacement::InsertFree(int slot) {
    bufTable[slot].next = free;
    free = slot;

    // Return ok
    return (0);
}

RC LRUPageReplacement::LinkHead(int slot) {
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

RC LRUPageReplacement::Unlink(int slot) {

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


//执行LRU算法，
RC LRUPageReplacement::execPageReplacement(int &slot) {

    RC rc ;
    //空闲块链不空，选择一个槽号从空闲块链中
    if(free != INVALID_SLOT){
        slot = free;
        free = bufTable[slot].next;
        cout<<"||空闲块链不空，将内存中信息读入slot块中";
    } else{
        //选择LRU 页且不被定住
        cout<<"||空闲块链为空，执行页面淘汰算法";
        for(slot = last ; slot != INVALID_SLOT ;slot = bufTable[slot].prev){
            if (bufTable[slot].pinCount == 0)
                break;
        }
        cout<<" || 淘汰的缓冲区槽号为 "<<slot<<"且对应的文件名描述符和页号分别为 : "<<bufTable[slot].fd<<"--"<<bufTable[slot].pageNum;

        if (slot == INVALID_SLOT)
            return (PF_NOBUF);

        //如果当前页是脏的写回磁盘

        if(bufTable[slot].bDirty){
            if ((rc = WritePage(bufTable[slot].fd, bufTable[slot].pageNum,
                                bufTable[slot].pData)))
                return (rc);

            bufTable[slot].bDirty = FALSE;
            cout<<"*****且该也页面为脏页要写回到磁盘\n";
        }
        // 把当前页从hash表和使用块链移除
        // Remove page from the hash table and slot from the used buffer list
        if ((rc = hashTable->Delete(bufTable[slot].fd, bufTable[slot].pageNum)) ||
            (rc = Unlink(slot)))
            return (rc);



    }

    //将该槽放入使用链首
    // Link slot at the head of the used list
    if ((rc = LinkHead(slot)))
        return (rc);

    // Return ok
    return (0);

}


