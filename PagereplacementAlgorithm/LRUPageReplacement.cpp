//
// Created by hp on 2022/10/11.
//

#include "LRUPageReplacement.h"

LRUPageReplacement::LRUPageReplacement(int hashTableSize)
        : PF_pageReplacement(hashTableSize) {
    cout<<"LRU�ڴ�����๹����ϣ����������ɸ��ฺ���PF_BufferPool�Ĺ���";
}

RC LRUPageReplacement::GetPage(int fd, PageNum pageNum, char *ppBuffer, int bMultiplePins) {

    RC rc;
    int slot;

    if((rc = hashTable->Find(fd,pageNum,slot)) &&
            (rc != PF_HASHNOTFOUND))
        return rc;
    //ҳ�����ڴ�
    if(rc == PF_HASHNOTFOUND){
        cout<<"���ʵ�ҳ�����ڴ�";
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

        cout<<"����ҳ���ڴ�,����ҳ���뵽ʹ������\n";
        cout<<"****************************************************************************************************"<<endl;
//        if (!bMultiplePins && bufTable[slot].pinCount > 0)
//            return (PF_PAGEPINNED);
        //ÿʹ��һ�ν����ҳ��ס
//        bufTable[slot].pinCount++;
//        cout<<bufTable[slot].pinCount<<endl;
        if((rc = Unlink(slot))|| (rc = LinkHead(slot)))
            return (rc);

    }
//    cout<<bufTable[slot].pData<<"�ҵ���getPage�������";
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


//ִ��LRU�㷨��
RC LRUPageReplacement::execPageReplacement(int &slot) {

    RC rc ;
    //���п������գ�ѡ��һ���ۺŴӿ��п�����
    if(free != INVALID_SLOT){
        slot = free;
        free = bufTable[slot].next;
        cout<<"||���п������գ����ڴ�����Ϣ����slot����";
    } else{
        //ѡ��LRU ҳ�Ҳ�����ס
        cout<<"||���п���Ϊ�գ�ִ��ҳ����̭�㷨";
        for(slot = last ; slot != INVALID_SLOT ;slot = bufTable[slot].prev){
            if (bufTable[slot].pinCount == 0)
                break;
        }
        cout<<" || ��̭�Ļ������ۺ�Ϊ "<<slot<<"�Ҷ�Ӧ���ļ�����������ҳ�ŷֱ�Ϊ : "<<bufTable[slot].fd<<"--"<<bufTable[slot].pageNum;

        if (slot == INVALID_SLOT)
            return (PF_NOBUF);

        //�����ǰҳ�����д�ش���

        if(bufTable[slot].bDirty){
            if ((rc = WritePage(bufTable[slot].fd, bufTable[slot].pageNum,
                                bufTable[slot].pData)))
                return (rc);

            bufTable[slot].bDirty = FALSE;
            cout<<"*****�Ҹ�Ҳҳ��Ϊ��ҳҪд�ص�����\n";
        }
        // �ѵ�ǰҳ��hash���ʹ�ÿ����Ƴ�
        // Remove page from the hash table and slot from the used buffer list
        if ((rc = hashTable->Delete(bufTable[slot].fd, bufTable[slot].pageNum)) ||
            (rc = Unlink(slot)))
            return (rc);



    }

    //���ò۷���ʹ������
    // Link slot at the head of the used list
    if ((rc = LinkHead(slot)))
        return (rc);

    // Return ok
    return (0);

}


