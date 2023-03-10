//
// rm_rid.h
//
//   The Record Id interface
//

#ifndef RM_RID_H
#define RM_RID_H

// We separate the interface of RID from the rest of RM because some
// components will require the use of RID but not the rest of RM.

#include "../redbase.h"

//
// PageNum: uniquely identifies a page in a file
//
typedef long long PageNum;

//
// SlotNum: uniquely identifies a record in a page
//
typedef int SlotNum;


//
// RID: Record id interface
//
class RID {
  static const PageNum INVALID_PAGE = -1;
  static const SlotNum INVALID_SLOT = -1;
public:
    RID();                                         // Default constructor
    RID(PageNum pageNum, SlotNum slotNum);
    ~RID();                                        // Destructor
    RID& operator= (const RID &rid);               // Copies RID
    bool operator== (const RID &rid) const;

    RC GetPageNum(PageNum &pageNum) const;         // Return page number
    RC GetSlotNum(SlotNum &slotNum) const;         // Return slot number
    RC setPageNum(PageNum pageNum) ;
    RC setSlotNum(SlotNum slotNum);

    RC isValidRID() const; // checks if it is a valid RID

private:
  PageNum page;
  SlotNum slot;
};


#endif
