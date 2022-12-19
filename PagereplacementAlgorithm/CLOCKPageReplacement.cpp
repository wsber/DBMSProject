//
// Created by hp on 2022/10/12.
//

#include "CLOCKPageReplacement.h"


CLOCKPageReplacement::CLOCKPageReplacement(int hashTableSize) : PF_pageReplacement(hashTableSize) {
    cout<<"CLOCK�ڴ�����๹����ϣ��������ڴ滺��صĹ����ɸ��ฺ��";
    clockPoint = first;
}

RC CLOCKPageReplacement::execPageReplacement(int &slot) {

    RC rc;
    if(free != INVALID_SLOT){
        slot =free;
        free = bufTable[slot].next;
        cout<<"||���п������գ����ڴ�����Ϣ����slot��"<<slot<<"��";
        //���ò۷���ʹ������
        // Link slot at the head of the used list
        if ((rc = LinkHead(slot)))
            return (rc);
    } else{
        //ѡ��CLOCKҳ�Ҳ�����ס
        cout<<"||���п���Ϊ�գ�ִ��ҳ����̭�㷨CLOCK: ";

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
        cout<<" || ��̭�Ļ������ۺ�Ϊ "<<slot<<"�Ҷ�Ӧ���ļ�����������ҳ�ŷֱ�Ϊ : "<<bufTable[slot].fd<<"--"<<bufTable[slot].pageNum;
        if(bufTable[slot].next != INVALID_SLOT)
            clockPoint = bufTable[slot].next ;
        else clockPoint = first ;

        if (slot == INVALID_SLOT)
            return (PF_NOBUF);

        //�����ǰҳ�����д�ش���

        if(bufTable[slot].bDirty){
            if ((rc = WritePage(bufTable[slot].fd, bufTable[slot].pageNum,
                                bufTable[slot].pData)))
                return (rc);
            bufTable[slot].bDirty = FALSE;
            cout<<"||�Ҹ�Ҳҳ��Ϊ��ҳҪд�ص�����\n";
        }
        // �ѵ�ǰҳ��hash���ʹ�ÿ����Ƴ�
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
    //ҳ�����ڴ�
    if(rc == PF_HASHNOTFOUND){

        cout<<"���ʵ�ҳ�����ڴ�,��ǰCLOCKָ��ָ��-->"<<clockPoint;
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
        cout<<"�ۺ�Ϊ"<<slot<<"��ҳ���ڴ�,�����ڴ��и�ҳ�����ݣ�����ҳ����λ r ��1\n";
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
