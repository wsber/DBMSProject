//
// File:        SM component
// Description: Print parameters of all SM_Manager methods
// Authors:     Yifei Huang (yifei@stanford.edu)
//

#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include "../redbase.h"
#include "sm.h"
#include "../IX/ix.h"
#include "../RM/rm.h"
#undef max
#include <vector>
#include <string>
#include <set>
#include "stddef.h"
#include "../statistics.h"
#include <cfloat>


using namespace std;

extern StatisticsMgr *pStatisticsMgr;
extern void PF_Statistics();
/* 
 * These functions are used to parse string forms of int, flaot or
 * string, and move them into the record object during load
 */
bool recInsert_int(char *location, string value, int length){
  int num;
  istringstream ss(value);
  ss >> num;
  if(ss.fail())
    return false;
  //printf("num: %d \n", num);
  memcpy(location, (char*)&num, length);
  return true;
}

bool recInsert_float(char *location, string value, int length){
  float num;
  istringstream ss(value);
  ss >> num;
  if(ss.fail())
    return false;
  memcpy(location, (char*)&num, length);
  return true;
}

bool recInsert_string(char *location, string value, int length){
  if(value.length() >= length){
    memcpy(location, value.c_str(), length);
    return true;
  }
  memcpy(location, value.c_str(), value.length()+1);
  return true;
}

/*
 * Constructor and destructor for SM_Manager
 */
SM_Manager::SM_Manager(IX_Manager &ixm, RM_Manager &rmm) : ixm(ixm), rmm(rmm){
  printIndex = false;
  useQO = true;
  calcStats = false;
  printPageStats = true;
}

SM_Manager::~SM_Manager()
{

}

/*
 * This deals with opening a folder that corresponds to the database
 * name and chdir-ing into that folder
 */
RC SM_Manager::OpenDb(const char *dbName)
{
  RC rc = 0;
  
  if(strlen(dbName) > MAX_DB_NAME){ // checks for valid dbName
    return (SM_INVALIDDB);
  }

  if(chdir(dbName) < 0){ // Checks for errors while Chdir-ing
    cerr << "Cannot chdir to " << dbName << "\n";
    return (SM_INVALIDDB);
  }

  // Open and keep the relcat and attrcat filehandles stored
  // during duration of database
  if((rc = rmm.OpenFile("relcat", relcatFH) )){
    return (SM_INVALIDDB);
  }
  if((rc = rmm.OpenFile("attrcat", attrcatFH))) {
    return (SM_INVALIDDB);
  }
  
  return (0);
}


/*
 * This deals with closing the database, so closing any
 * open files. In this case, we close relcat and attrcat
 */
RC SM_Manager::CloseDb()
{
  
  RC rc = 0;
  if((rc = rmm.CloseFile(relcatFH) )){
    return (rc);
  }
  if((rc = rmm.CloseFile(attrcatFH))){
    return (rc);
  } 
  
  return (0);
}


/*
 * This function returns true if the attribute type
 * and attribute length are compatible
 */
bool SM_Manager::isValidAttrType(AttrInfo attribute){

  AttrType type = attribute.attrType;
  int length = attribute.attrLength;
  if(type == INT && length == 4)
    return true;
  if(type == FLOAT && length == 4)
    return true;
  if(type == STRING && (length > 0) && length < MAXSTRINGLEN)
    return true;

  return false;
}

/*
 * This function creates a table. It first checks that the relation
 * name and its attributes are all valid, and it then adds its specs
 * to relcat and attrcat and creates the file associated with
 * this table
 */
RC SM_Manager::CreateTable(const char *relName,
                           int        attrCount,
                           AttrInfo   *attributes)
{
  cout << "CreateTable\n"
    << "   relName     =" << relName << "\n"
    << "   attrCount   =" << attrCount << "\n";
  for (int i = 0; i < attrCount; i++)
    cout << "   attributes[" << i << "].attrName=" << attributes[i].attrName
        << "   attrType="
        << (attributes[i].attrType == INT ? "INT" :
            attributes[i].attrType == FLOAT ? "FLOAT" : "STRING")
        << "   attrLength=" << attributes[i].attrLength << "\n";

  RC rc = 0;
  set<string> relAttributes;

  // Check for appropraite # of attributes
  if(attrCount > MAXATTRS || attrCount < 1){
    printf("reaches here\n");
    return (SM_BADREL);
  }
  if(strlen(relName) > MAXNAME) // Check for valid relName size
    return (SM_BADRELNAME);

  // Check the attribute specifications
  int totalRecSize = 0;
  for(int i = 0; i < attrCount; i++){
    if(strlen(attributes[i].attrName) > MAXNAME) // check name size
      return (SM_BADATTR);
    if(! isValidAttrType(attributes[i])) // check type
      return (SM_BADATTR);
    totalRecSize += attributes[i].attrLength; 
    string attrString(attributes[i].attrName); // check attribute dups
    bool exists = (relAttributes.find(attrString) != relAttributes.end());
    if(exists)
      return (SM_BADREL);
    else
      relAttributes.insert(attrString);
  }

  // Passed error check. Now information in relcat and attrcat

  // Create a file for this relation. This will check for duplicate tables
  // of the same name.
  if((rc = rmm.CreateFile(relName, totalRecSize)))
    return (SM_BADRELNAME);

  // For each attribute, insert into attrcat:
  RID rid;
  int currOffset = 0;
  for(int i = 0; i < attrCount; i++){
    AttrInfo attr = attributes[i];
    if((rc = InsertAttrCat(relName, attr, currOffset, i)))
      return (rc);
    currOffset += attr.attrLength;
  }
    
  // Insert into RelCat
  if((rc = InsertRelCat(relName, attrCount, totalRecSize)))
    return (rc);

  // Make sure changes to attrcat and relcat are reflected
  if((rc = attrcatFH.ForcePages()) || (rc = relcatFH.ForcePages()))
    return (rc);

  return (0);
}

/*
 * This function inserts a relation entry into relcat. 
 */
RC SM_Manager::InsertRelCat(const char *relName, int attrCount, int recSize){
  RC rc = 0;

  RelCatEntry* rEntry = (RelCatEntry *) malloc(sizeof(RelCatEntry));
  memset((void*)rEntry, 0, sizeof(*rEntry));
  *rEntry = (RelCatEntry) {"\0", 0, 0, 0, 0};
  memcpy(rEntry->relName, relName, MAXNAME + 1); // name
  rEntry->tupleLength = recSize;                 // record size
  rEntry->attrCount = attrCount;                 // # of attributes
  rEntry->indexCount = 0;             // starting # of incides
  rEntry->indexCurrNum = 0;           // starting enumeration of indices
  // FOR EX component
  rEntry->numTuples = 0;
  rEntry->statsInitialized = false;

  // Insert into relcat
  RID relRID;
  rc = relcatFH.InsertRec((char *)rEntry, relRID);
  free(rEntry);

  return rc;
}

/*
 * This function inserts an attribute into attrcat
 */
RC SM_Manager::InsertAttrCat(const char *relName, AttrInfo attr, int offset, int attrNum){
  RC rc = 0;
  
  AttrCatEntry *aEntry = (AttrCatEntry *)malloc(sizeof(AttrCatEntry));
  memset((void*)aEntry, 0, sizeof(*aEntry));
  *aEntry = (AttrCatEntry) {"\0", "\0", 0, INT, 0, 0, 0};
  memcpy(aEntry->relName, relName, MAXNAME + 1);        // relation anme
  memcpy(aEntry->attrName, attr.attrName, MAXNAME + 1); // attribute name
  aEntry->offset = offset;                // attribute offset
  aEntry->attrType = attr.attrType;       // type
  aEntry->attrLength = attr.attrLength;   // length
  aEntry->indexNo = NO_INDEXES;           // index number
  aEntry->attrNum = attrNum;              // attribute # in sequence for this relation
  // For EX component
  aEntry->numDistinct = 0;
  aEntry->maxValue = FLT_MIN;
  aEntry->minValue = FLT_MAX;

  // Do insertion
  RID attrRID;
  rc = attrcatFH.InsertRec((char *)aEntry, attrRID);
  free(aEntry);
  
  
  return rc;

}


/*
 * This destroys a table, all its indices, and removes the contents
 * from relcat and attrcat
 */
RC SM_Manager::DropTable(const char *relName)
{
  cout << "DropTable\n   relName=" << relName << "\n";
  RC rc = 0;

  if(strlen(relName) > MAXNAME) // check for whether this is a valid name
    return (SM_BADRELNAME);
  // Try to destroy the table. This should detect whether the file is there
  if((rc = rmm.DestroyFile(relName))){
    return (SM_BADRELNAME);
  }

  // Retrieve the record associated with the relation
  RM_Record relRec;
  RelCatEntry *relEntry;
  if((rc = GetRelEntry(relName, relRec, relEntry)))
    return (rc);
  int numAttr = relEntry->attrCount;

  // Retrieve all its attributes
  SM_AttrIterator attrIt;
  if((rc = attrIt.OpenIterator(attrcatFH, const_cast<char*>(relName))))
    return (rc);
  AttrCatEntry *attrEntry;
  RM_Record attrRec;
  for(int i=0; i < numAttr; i++){
    // Get the next attribute
    if((rc = attrIt.GetNextAttr(attrRec, attrEntry))){
      return (rc);
    }
    // Check whether it has indices. If so, delete it
    if((attrEntry->indexNo != NO_INDEXES)){
      if((rc = DropIndex(relName, attrEntry->attrName)))
        return (rc);
    }
    // Delete that attribute record
    RID attrRID;
    if((rc = attrRec.GetRid(attrRID)) || (rc = attrcatFH.DeleteRec(attrRID)))
      return (rc);

  }
  if((rc = attrIt.CloseIterator()))
    return (rc);

  // Delete the record associated with the relation
  RID relRID;
  if((rc = relRec.GetRid(relRID)) || (rc = relcatFH.DeleteRec(relRID)))
    return (rc);

  return (0);
}

/*
 * Retrieves the record and data associated with a specific relation in relcat
 */
RC SM_Manager::GetRelEntry(const char *relName, RM_Record &relRec, RelCatEntry *&entry){
  RC rc = 0;
  // Use a scan to search for it
  RM_FileScan fs;
  if((rc = fs.OpenScan(relcatFH, STRING, MAXNAME+1, 0, EQ_OP, const_cast<char*>(relName))))
    return (rc);
  if((rc = fs.GetNextRec(relRec))) // there should be only one entry
    return (SM_BADRELNAME);
  
  if((rc = fs.CloseScan())) 
    return (rc);

  if((rc = relRec.GetData((char *&)entry))) // retrieve its data contents
    return (rc);

  return (0);
}

/*
 * Given a list of relations, it retrieves all the relCatEntries associated with them placing
 * them in the list specified by relEntries. It also returns the total # of attributes in all the
 * relations combined, and populates the mapping from relation name to index number in relEntries
 */
RC SM_Manager::GetAllRels(RelCatEntry *relEntries, int nRelations, const char * const relations[],
  int &attrCount, map<string, int> &relToInt){
  RC rc = 0;
  for(int i=0; i < nRelations; i++){
    RelCatEntry *rEntry;
    RM_Record rec;
    if((rc = GetRelEntry(relations[i], rec, rEntry))) // retrieve this entry
      return (rc);
    *(relEntries + i) = (RelCatEntry) {"\0", 0, 0, 0, 0};
    memcpy((char *)(relEntries + i), (char *)rEntry, sizeof(RelCatEntry)); // copy it into appropraite spot
    attrCount += relEntries[i].attrCount;

    // create a map from relation name to # in order
    string relString(relEntries[i].relName);
    relToInt.insert({relString, i});
  }
  return (rc);
}

/*
 * Given a RelCatEntry, it populates aEntry with information about all its attribute.
 * While doing so, it also updates the attribute-to-relation mapping
 */
RC SM_Manager::GetAttrForRel(RelCatEntry *relEntry, AttrCatEntry *aEntry, std::map<std::string, std::set<std::string> > &attrToRel){
  RC rc = 0;
  // Iterate through all the attributes in this relation
  SM_AttrIterator attrIt;
  if((rc = attrIt.OpenIterator(attrcatFH, (relEntry->relName))))
    return (rc);
  RM_Record attrRec;
  AttrCatEntry *attrEntry;
  for(int i = 0; i < relEntry->attrCount; i++){
    // For each attribute, get its AttrCatEntry
    if((rc = attrIt.GetNextAttr(attrRec, attrEntry)))
      return (rc);
    
    int slot = attrEntry->attrNum;
    *(aEntry + slot) = (AttrCatEntry) {"\0", "\0", 0, INT, 0, 0, 0};
    memcpy((char *)(aEntry + slot), (char *)attrEntry, sizeof(AttrCatEntry));

    // add this attribute to the mapping from attribute name to set of relations with this attribute name
    string attrString(aEntry[slot].attrName);
    string relString(relEntry->relName);
    map<string, set<string> >::iterator it = attrToRel.find(attrString);
    if(it == attrToRel.end()){ // if there isn't a set already for this attribute, create it
      set<string> relNames;
      relNames.insert(relString);
      attrToRel.insert({attrString, relNames});
    }
    else{ // otherwise, just add it.
      attrToRel[attrString].insert(relString);
    }
  }
  if((rc = attrIt.CloseIterator())) // Done with search!
    return (rc);

  return (0);
}



/*
 * This creates an index on the relation, and adds all the current
 * contents of the relation into this index
 */
RC SM_Manager::CreateIndex(const char *relName,
                           const char *attrName)
{
  cout << "CreateIndex\n"
    << "   relName =" << relName << "\n"
    << "   attrName=" << attrName << "\n";

  RC rc = 0;
  RM_Record relRec;
  RelCatEntry *rEntry;
  if((rc = GetRelEntry(relName, relRec, rEntry))) // get the relation info
    return (rc);

  // Find the attribute associated with this index
  RM_Record attrRec; 
  AttrCatEntry *aEntry;
  if((rc = FindAttr(relName, attrName, attrRec, aEntry))){
    return (rc);
  }

  // check there isnt already an index
  if(aEntry->indexNo != NO_INDEXES)
    return (SM_INDEXEDALREADY);

  // Create this index
  if((rc = ixm.CreateIndex(relName, rEntry->indexCurrNum, aEntry->attrType, aEntry->attrLength)))
    return (rc);

  // Gets ready to scan through the file associated with the relation
  IX_IndexHandle ih;
  RM_FileHandle fh;
  RM_FileScan fs;
  if((rc = ixm.OpenIndex(relName, rEntry->indexCurrNum, ih)))
    return (rc);
  if((rc = rmm.OpenFile(relName, fh)))
    return (rc);

  // scan through the entire file:
  if((rc = fs.OpenScan(fh, INT, 4, 0, NO_OP, NULL))){
    return (rc);
  }
  RM_Record rec;
  while(fs.GetNextRec(rec) != RM_EOF){
    char *pData;
    RID rid;
    if((rc = rec.GetData(pData) || (rc = rec.GetRid(rid)))) // retrieve the record
      return (rc);
    if((rc = ih.InsertEntry(pData+ aEntry->offset, rid))) // insert into index
      return (rc);
  }
  if((rc = fs.CloseScan()) || (rc = rmm.CloseFile(fh)) || (rc = ixm.CloseIndex(ih)))
    return (rc);
  // Close all scans, indices and files
  
  // rewrite entry for attribute and relation in attrcat and relcat
  aEntry->indexNo = rEntry->indexCurrNum;
  rEntry->indexCurrNum++;
  rEntry->indexCount++;

  // write both back
  if((rc = relcatFH.UpdateRec(relRec)) || (rc = attrcatFH.UpdateRec(attrRec)))
    return (rc);
  if((rc = relcatFH.ForcePages() || (rc = attrcatFH.ForcePages())))
    return (rc);

  return (0);
}

/*
 * This function returns a record and its data for a particular
 * attribute in a particular relation
 */
RC SM_Manager::FindAttr(const char *relName, const char *attrName, RM_Record &attrRec, AttrCatEntry *&entry){
  RC rc = 0;
  RM_Record relRec;
  RelCatEntry * rEntry;
  if((rc = GetRelEntry(relName, relRec, rEntry))) // get the relation
    return (rc);
  
  // Iterate through attrcat for attributes in this relation
  SM_AttrIterator attrIt;
  if((rc = attrIt.OpenIterator(attrcatFH, const_cast<char*>(relName))))
    return (rc);
  bool notFound = true;
  while(notFound){
    if((RM_EOF == attrIt.GetNextAttr(attrRec, entry))){
      break;
    }
    // Check if the attribute names match
    if(strncmp(entry->attrName, attrName, MAXNAME + 1) == 0){
      notFound = false;
      break;
    }
  }
  if((rc = attrIt.CloseIterator())) // Done with search!
    return (rc);

  if(notFound == true)  // If attribute is not found, return error
    return (SM_INVALIDATTR);
  return (rc);

}


/*
 * This function destroys a valid index
 */
RC SM_Manager::DropIndex(const char *relName,
                         const char *attrName)
{
  cout << "DropIndex\n"
    << "   relName =" << relName << "\n"
    << "   attrName=" << attrName << "\n";

  RC rc = 0;
  RM_Record relRec;
  RelCatEntry *rEntry;
  if((rc = GetRelEntry(relName, relRec, rEntry))) // retrieve relation
    return (rc);

  RM_Record attrRec; // Finds the appropriate attribute
  AttrCatEntry *aEntry;
  if((rc = FindAttr(relName, attrName, attrRec, aEntry))){
    return (rc);
  }

  if((aEntry->indexNo == NO_INDEXES)) // Check that there is actually an index
    return (SM_NOINDEX);
  
  // Destroys the index
  if((rc = ixm.DestroyIndex(relName, aEntry->indexNo)))
    return (rc);

  // Update entries in the relation and attribute records
  aEntry->indexNo = NO_INDEXES;
  rEntry->indexCount--;

  // write both catalog pages back
  if((rc = relcatFH.UpdateRec(relRec)) || (rc = attrcatFH.UpdateRec(attrRec)))
    return (rc);
  if((rc = relcatFH.ForcePages() || (rc = attrcatFH.ForcePages())))
    return (rc);

  return (0);
}

/*
 * This sets up the Attr list, which is a struct used to hold information
 * about the attributes to facilitate loading files
 */
RC SM_Manager::PrepareAttr(RelCatEntry *rEntry, Attr* attributes){
  RC rc = 0; 
  // Iterate through the attributes related to this relation
  SM_AttrIterator attrIt;
  if((rc = attrIt.OpenIterator(attrcatFH, rEntry->relName)))
    return (rc);
  RM_Record attrRec;
  AttrCatEntry *aEntry;
  for(int i = 0; i < rEntry->attrCount; i++){
    if((rc = attrIt.GetNextAttr(attrRec, aEntry)))
      return (rc);
    // For each attribute, place its information in the appropriate slot
    int slot = aEntry->attrNum;
    attributes[slot].offset = aEntry->offset;
    attributes[slot].type = aEntry->attrType;
    attributes[slot].length = aEntry->attrLength;
    attributes[slot].indexNo = aEntry->indexNo;
    attributes[slot].numDistinct = aEntry->numDistinct;
    attributes[slot].maxValue = aEntry->maxValue;
    attributes[slot].minValue = aEntry->minValue;

    // Open the index if there is one associated with it
    if((aEntry->indexNo != NO_INDEXES)){
      IX_IndexHandle indexHandle;
      attributes[slot].ih = indexHandle;
      if((rc = ixm.OpenIndex(rEntry->relName, aEntry->indexNo, attributes[slot].ih)))
        return (rc);
    }

    // Make sure its parser is pointint to the appropriate parser depending
    // on its attribute type
    if(aEntry->attrType == INT){
      attributes[slot].recInsert = &recInsert_int;
    }
    else if(aEntry->attrType == FLOAT)
      attributes[slot].recInsert = &recInsert_float;
    else
      attributes[slot].recInsert = &recInsert_string;
  }
  if((rc = attrIt.CloseIterator()))
    return (rc);
  return (0);
}

/*
 * This loads a contents from a specified file into a specified relation
 */
RC SM_Manager::Load(const char *relName,
                    const char *fileName)
{
    cout << "Load\n"
         << "   relName =" << relName << "\n"
         << "   fileName=" << fileName << "\n";

  RC rc = 0;
  RM_Record relRec;
  RelCatEntry *rEntry;
  if((rc = GetRelEntry(relName, relRec, rEntry))) // retrieve the relation
    return (rc);
  if(rEntry->statsInitialized == false)
    calcStats = true;

  // Creates a struct containing info about the attributes to 
  // help with loading
  Attr* attributes = (Attr *)malloc(sizeof(Attr)*rEntry->attrCount);
  for(int i=0; i < rEntry->attrCount; i++){
    memset((void*)&attributes[i], 0, sizeof(attributes[i]));
    IX_IndexHandle ih;
    attributes[i] = (Attr) {0, 0, 0, 0, ih, recInsert_string, 0, FLT_MAX, FLT_MIN};
  }
  if((rc = PrepareAttr(rEntry, attributes)))
    return (rc);

  // Open the file and load contents
  RM_FileHandle relFH;
  if((rc = rmm.OpenFile(relName, relFH)))
    return (rc);
  int totalRecs = 0;
  rc = OpenAndLoadFile(relFH, fileName, attributes, rEntry->attrCount,
    rEntry->tupleLength, totalRecs);
  RC rc2;

  // write back attribute and rel stats;
  if(calcStats){
    rEntry->numTuples = totalRecs;
    rEntry->statsInitialized = true;
    if((rc = relcatFH.UpdateRec(relRec)) || (rc = relcatFH.ForcePages()))
      return (rc);

    SM_AttrIterator attrIt;
    if((rc = attrIt.OpenIterator(attrcatFH, rEntry->relName)))
      return (rc);
    RM_Record attrRec;
    AttrCatEntry *aEntry;
    for(int i = 0; i < rEntry->attrCount; i++){
      if((rc = attrIt.GetNextAttr(attrRec, aEntry)))
        return (rc);
      // For each attribute, place its information in the appropriate slot
      int slot = aEntry->attrNum;
      aEntry->minValue = attributes[slot].minValue;
      aEntry->maxValue = attributes[slot].maxValue;
      aEntry->numDistinct = attributes[slot].numDistinct;
      if((rc = attrcatFH.UpdateRec(attrRec)))
        return (rc);
    }
    if((rc = attrIt.CloseIterator()))
      return (rc);
    if((rc = attrcatFH.ForcePages()))
      return (rc);
    calcStats = false;
  }

  // Destroy and close the pointers in Attr struct
  if((rc2 = CleanUpAttr(attributes, rEntry->attrCount)))
    return (rc2);

  if((rc2 = rmm.CloseFile(relFH))) // Close the file
    return (rc2);

  return (rc);
}

/*
 * This function takes in the filehandle to the relation table, the load file,
 * info about the relation's attributes, and the record length, and
 * loads info from the file into the table
 *
 * If the load file has less attributes, return an error
 * If the load file has invalid int or float values, return an error
 * Other abnormalities, like too many tuples, or strings that are too long
 * will be dealt with by truncation, and no error will be returned
 */
RC SM_Manager::OpenAndLoadFile(RM_FileHandle &relFH, const char *fileName, Attr* attributes, int attrCount, 
  int recLength, int &loadedRecs){
  RC rc = 0;
  loadedRecs = 0;

  char *record = (char *)calloc(recLength, 1);

  // Open load file
  ifstream f(fileName);
  if(f.fail()){
    cout << "cannot open file :( " << endl;
    free(record);
    return (SM_BADLOADFILE);
  }

  vector<set<string> > numDistinct(attrCount);
 
  string line, token;
  string delimiter = ","; // tuples separated by comma
  while (getline(f, line)) { // read in load file one line at a time
    RID recRID;
    for(int i=0; i <attrCount; i++){ // expect a tuple per attribute specified
      if(line.size() == 0){
        free(record);
        f.close();
        return (SM_BADLOADFILE);
      }
      size_t pos = line.find(delimiter); // Find the value of the next delimiter
      if(pos == string::npos)            // and truncate it
        pos = line.size();
      token = line.substr(0, pos);
      line.erase(0, pos + delimiter.length());

      // Parse the attribute value, and insert it into the right slot.
      // If parsing is bad, recInsert should return false;
      if(attributes[i].recInsert(record + attributes[i].offset, token, attributes[i].length) == false){
        rc = SM_BADLOADFILE;
        free(record);
        f.close();
        return (rc);
      }
    }
    // Insert the record into the file
    
    if((rc = relFH.InsertRec(record, recRID))){
      free(record);
      f.close();
      return (rc);
    }

    // Insert the portions of the record into the appropriate indices
    for(int i=0; i < attrCount; i++){
      if(attributes[i].indexNo != NO_INDEXES){
        if((rc = attributes[i].ih.InsertEntry(record + attributes[i].offset, recRID))){
          free(record);
          f.close();
          return (rc);
        }
      }
      if(calcStats){
        int offset = attributes[i].offset;
        string attr(record + offset, record + offset + attributes[i].length);
        numDistinct[i].insert(attr);
        float attrValue = 0.0;
        if(attributes[i].type == STRING)
          attrValue = ConvertStrToFloat(record + offset);
        else if(attributes[i].type == INT)
          attrValue = (float) *((int*) (record + offset));
        else{
          attrValue = *((float*) (record + offset));
        }
        if(attrValue > attributes[i].maxValue)
          attributes[i].maxValue = attrValue;
        if(attrValue < attributes[i].minValue)
          attributes[i].minValue = attrValue;
      }

      
    }
    loadedRecs++;
    //printf("record : %d, %d\n", *(int*)record, *(int*)(record+4));
  }
  for(int i=0; i < attrCount; i++){
    attributes[i].numDistinct = numDistinct[i].size();
    //printf("num attributes: %d for index %d \n", attributes[i].numDistinct, i);
  }


cleanup:
  free(record);
  f.close();

  return (rc);
}

/*
 * This cleans up the struct of attributes used for loading values.
 * It closes any open indices, and frees the Attr* list
 */
RC SM_Manager::CleanUpAttr(Attr* attributes, int attrCount){
  RC rc = 0;
  for(int i=0; i < attrCount; i++){
    if(attributes[i].indexNo != NO_INDEXES){
      if((rc = ixm.CloseIndex(attributes[i].ih)))
        return (rc);
    }
  }
  free(attributes);
  return (rc);
}

/*
 * This function prints all the tuples inside of a relation
 */
RC SM_Manager::Print(const char *relName)
{
  cout << "Print\n"
    << "   relName=" << relName << "\n";

  RC rc = 0;
  RM_Record relRec;
  RelCatEntry *relEntry;
  if((rc = GetRelEntry(relName, relRec, relEntry))) // retrieves relation info
    return (SM_BADRELNAME);
  int numAttr = relEntry->attrCount;

  // Sets up the DataAttrInfo for printing
  DataAttrInfo * attributes = (DataAttrInfo *)malloc(numAttr* sizeof(DataAttrInfo));
  if((rc = SetUpPrint(relEntry, attributes)))
    return (rc);

  Printer printer(attributes, relEntry->attrCount);
  printer.PrintHeader(cout);

  // open the file, and a scan through the entire file
  RM_FileHandle fh;
  RM_FileScan fs;
  if((rc = rmm.OpenFile(relName, fh)) || (rc = fs.OpenScan(fh, INT, 4, 0, NO_OP, NULL))){
    free(attributes);
    return (rc);
  }

  // Retrieve each record and print it
  RM_Record rec;
  while(fs.GetNextRec(rec) != RM_EOF){
    char *pData;
    if((rec.GetData(pData))){
      free(attributes);
      return (rc);
    }
    printer.Print(cout, pData);
  }
  fs.CloseScan();

  printer.PrintFooter(cout);
  free(attributes); // free DataAttrInfo

  return (0);
}

/*
 * This iterates through the attributes in a relation, and sets up 
 * the DataAttrInfo for printing
 */
RC SM_Manager::SetUpPrint(RelCatEntry* rEntry, DataAttrInfo *attributes){
  RC rc = 0;
  RID attrRID;
  RM_Record attrRec;
  AttrCatEntry *aEntry;

  // Iterate through attrcat for attributes related to this relation
  SM_AttrIterator attrIt;
  if((rc = attrIt.OpenIterator(attrcatFH, rEntry->relName)))
    return (rc);

  for(int i=0; i < rEntry->attrCount; i++){
    if((rc = attrIt.GetNextAttr(attrRec, aEntry))){
      return (rc);
    }
    int slot = aEntry->attrNum; // insert its info in the appropriate slot

    memcpy(attributes[slot].relName, aEntry->relName, MAXNAME + 1);
    memcpy(attributes[slot].attrName, aEntry->attrName, MAXNAME + 1);
    attributes[slot].offset = aEntry->offset;
    attributes[slot].attrType = aEntry->attrType;
    attributes[slot].attrLength = aEntry->attrLength;
    attributes[slot].indexNo = aEntry->indexNo;
  }
  if((rc = attrIt.CloseIterator()))
    return (rc);

  return (rc);
}

/*
 * This sets printIndex to true or false, depending on what's specified
 * Otherwise, any other call will do nothing.
 */
RC SM_Manager::Set(const char *paramName, const char *value)
{
    RC rc = 0;
    cout << "Set\n"
         << "   paramName=" << paramName << "\n"
         << "   value    =" << value << "\n";
    if(strncmp(paramName, "printIndex", 10) == 0 && strncmp(value, "true", 4) ==0){
      printIndex = true;
      return (0);
    }
    else if(strncmp(paramName, "printIndex", 10) == 0 && strncmp(value, "false", 5) ==0){
      printIndex = false;
      return (0);
    }
    if(strncmp(paramName, "printPageStats", 14) == 0  && strncmp(value, "true", 4) == 0){
      printPageStats = true;
      return (0);
    }
    if(strncmp(paramName, "printPageStats", 14) == 0  && strncmp(value, "false", 4) == 0){
      printPageStats = false;
      return (0);
    }
    if(strncmp(paramName, "printPageStats", 14) == 0 ){
      int *piGP = pStatisticsMgr->Get(PF_GETPAGE);
      int *piPF = pStatisticsMgr->Get(PF_PAGEFOUND);
      int *piPNF = pStatisticsMgr->Get(PF_PAGENOTFOUND);

      cout << "PF Layer Statistics" << endl;
      cout << "-------------------" << endl;
      if(piGP)
        cout << "Total number of calls to GetPage Routine: " << *piGP << endl;
      else
        cout << "Total number of calls to GetPage Routine: None" << endl;
      if(piPF)
        cout << "  Number found: " << *piPF << endl;
      else
        cout << "  Number found: None" << endl;
      if(piPNF)
        cout << "  Number not found: " << *piPNF << endl;
      else
        cout << "  Number found: None" << endl;
      return (0);
    }
    if(strncmp(paramName, "resetPageStats", 14) == 0){
      pStatisticsMgr->Reset();
      return (0);
    }

    if(strncmp(paramName, "useQO", 5) == 0 && strncmp(value, "true", 4) ==0){
      cout << "Using QO" << endl;
      useQO = true;
      return (0);
    }
    if(strncmp(paramName, "useQO", 5) == 0 && strncmp(value, "false", 5) ==0){
      cout << "disabling QO" << endl;
      useQO = false;
      return (0);
    }
    if(strncmp(paramName, "printStats", 10) == 0){
      PrintStats(value);
      return (0);
    }
    if(strncmp(paramName, "calcStats", 9) == 0){
      CalcStats(value);
      return (0);
    }


    return (SM_BADSET);
}

RC SM_Manager::ResetPageStats(){
  pStatisticsMgr->Reset();
  return (0);
}

RC SM_Manager::PrintPageStats(){
  int *piGP = pStatisticsMgr->Get(PF_GETPAGE);
  int *piPF = pStatisticsMgr->Get(PF_PAGEFOUND);
  int *piPNF = pStatisticsMgr->Get(PF_PAGENOTFOUND);

  cout << "PF Layer Statistics" << endl;
  cout << "-------------------" << endl;
  if(piGP)
    cout << "Total number of calls to GetPage Routine: " << *piGP << endl;
  else
    cout << "Total number of calls to GetPage Routine: None" << endl;
  if(piPF)
    cout << "  Number found: " << *piPF << endl;
  else
    cout << "  Number found: None" << endl;
  if(piPNF)
    cout << "  Number not found: " << *piPNF << endl;
  else
    cout << "  Number found: None" << endl;
  return (0);
}

/*
 * Prints info about all the relations in this database
 * The info printed includes:
 * relName
 * tupleLength
 * attrCount
 * indexCount
 */
RC SM_Manager::Help()
{
  cout << "Help\n";
  RC rc = 0;
  // Malloc DataAttrInfo for printing, and set it up
  DataAttrInfo * attributes = (DataAttrInfo *)malloc(4* sizeof(DataAttrInfo));
  if((rc = SetUpRelCatAttributes(attributes)))
    return (rc);
  Printer printer(attributes, 4);
  printer.PrintHeader(cout);

  RM_FileScan fs;
  if((rc = fs.OpenScan(relcatFH, INT, 4, 0, NO_OP, NULL))){
    free(attributes);
    return (rc);
  }

  RM_Record rec; // iterate through all relation entries
  while(fs.GetNextRec(rec) != RM_EOF){
    char *pData;
    if((rec.GetData(pData))){
      free(attributes);
      return (rc);
    }
    printer.Print(cout, pData);
  }

  fs.CloseScan();
  printer.PrintFooter(cout);
  free(attributes);

  return (0);
}

/*
 * This sets up the dataAttrInfo struct for printing from relcat
 */
RC SM_Manager::SetUpRelCatAttributes(DataAttrInfo *attributes){
  int numAttr = 4;
  for(int i= 0; i < numAttr; i++){
    memcpy(attributes[i].relName, "relcat", strlen("relcat") + 1);
    attributes[i].indexNo = 0;
  }
  
  memcpy(attributes[0].attrName, "relName", MAXNAME + 1);
  memcpy(attributes[1].attrName, "tupleLength", MAXNAME + 1);
  memcpy(attributes[2].attrName, "attrCount", MAXNAME + 1);
  memcpy(attributes[3].attrName, "indexCount", MAXNAME + 1);

  attributes[0].offset = (int) offsetof(RelCatEntry,relName);
  attributes[1].offset = (int) offsetof(RelCatEntry,tupleLength);
  attributes[2].offset = (int) offsetof(RelCatEntry,attrCount);
  attributes[3].offset = (int) offsetof(RelCatEntry,indexCount);

  attributes[0].attrType = STRING;
  attributes[1].attrType = INT;
  attributes[2].attrType = INT;
  attributes[3].attrType = INT;

  attributes[0].attrLength = MAXNAME + 1;
  attributes[1].attrLength = 4;
  attributes[2].attrLength = 4;
  attributes[3].attrLength = 4;

  return (0);
}

/*
 * Prints info about all the attributes in a specified relation
 * The info printed includes:
 * relName
 * attributeName
 * offset
 * attribute type
 * attribute lenght
 * indexNo
 */
RC SM_Manager::Help(const char *relName)
{
    cout << "Help\n"
         << "   relName=" << relName << "\n";
  RC rc = 0;
  RM_FileScan fs;
  RM_Record rec;

  // Check that this relation exists:
  if((rc = fs.OpenScan(relcatFH, STRING, MAXNAME+1, 0, EQ_OP, const_cast<char*>(relName))))
    return (rc);
  if(fs.GetNextRec(rec) == RM_EOF){
    fs.CloseScan();
    return (SM_BADRELNAME);
  }
  fs.CloseScan();
  

  // Sets up the DataAttrInfo for printing
  DataAttrInfo * attributes = (DataAttrInfo *)malloc(6* sizeof(DataAttrInfo));
  if((rc = SetUpAttrCatAttributes(attributes)))
    return (rc);
  Printer printer(attributes, 6);
  printer.PrintHeader(cout);

  // Iterate through attrcat to find all attributes
  // associated with this relation. And print them.
  if((rc = fs.OpenScan(attrcatFH, STRING, MAXNAME+1, 0, EQ_OP, const_cast<char*>(relName))))
    return (rc);

  while(fs.GetNextRec(rec) != RM_EOF){
    char *pData;
    if((rec.GetData(pData)))
      return (rc);
    printer.Print(cout, pData);
  }

  if((rc = fs.CloseScan() ))
    return (rc);

  // If we are to print the index, itereate through again, and print
  // the entire index
  if((rc = fs.OpenScan(attrcatFH, STRING, MAXNAME+1, 0, EQ_OP, const_cast<char*>(relName))))
    return (rc);
  while(fs.GetNextRec(rec) != RM_EOF){
    char *pData;
    if((rec.GetData(pData)))
      return (rc);
    if(printIndex){
    AttrCatEntry *attr = (AttrCatEntry*)pData;
      if((attr->indexNo != NO_INDEXES)){
        IX_IndexHandle ih;
        if((rc = ixm.OpenIndex(relName, attr->indexNo, ih)))
          return (rc);
        //printf("successfully opens \n");
        if((rc = ih.PrintIndex()) || (rc = ixm.CloseIndex(ih)))
          return (rc);
      }
    }
  }

  printer.PrintFooter(cout);
  free(attributes);
  return (0);
}

/*
 * This sets up the dataAttrInfo struct for printing from attrcat
 */
RC SM_Manager::SetUpAttrCatAttributes(DataAttrInfo *attributes){
  int numAttr = 6;
  for(int i= 0; i < numAttr; i++){
    memcpy(attributes[i].relName, "attrcat", strlen("attrcat") + 1);
    attributes[i].indexNo = 0;
  }
  
  memcpy(attributes[0].attrName, "relName", MAXNAME + 1);
  memcpy(attributes[1].attrName, "attrName", MAXNAME + 1);
  memcpy(attributes[2].attrName, "offset", MAXNAME + 1);
  memcpy(attributes[3].attrName, "attrType", MAXNAME + 1);
  memcpy(attributes[4].attrName, "attrLength", MAXNAME + 1);
  memcpy(attributes[5].attrName, "indexNo", MAXNAME + 1);

  attributes[0].offset = (int) offsetof(AttrCatEntry,relName);
  attributes[1].offset = (int) offsetof(AttrCatEntry,attrName);
  attributes[2].offset = (int) offsetof(AttrCatEntry,offset);
  attributes[3].offset = (int) offsetof(AttrCatEntry,attrType);
  attributes[4].offset = (int) offsetof(AttrCatEntry,attrLength);
  attributes[5].offset = (int) offsetof(AttrCatEntry,indexNo);

  attributes[0].attrType = STRING;
  attributes[1].attrType = STRING;
  attributes[2].attrType = INT;
  attributes[3].attrType = INT;
  attributes[4].attrType = INT;
  attributes[5].attrType = INT;

  attributes[0].attrLength = MAXNAME + 1;
  attributes[1].attrLength = MAXNAME + 1;
  attributes[2].attrLength = 4;
  attributes[3].attrLength = 4;
  attributes[4].attrLength = 4;
  attributes[5].attrLength = 4;

  return (0);
}

float SM_Manager::ConvertStrToFloat(char *string){
  float value = (float) string[0];
  return value;
}

RC SM_Manager::PrintStats(const char *relName){
  RC rc = 0;
  cout << "Printing stats for relation " << relName << endl;
  if(strlen(relName) > MAXNAME) // check for whether this is a valid name
    return (SM_BADRELNAME);

  // Retrieve the record associated with the relation
  RM_Record relRec;
  RelCatEntry *relEntry;
  if((rc = GetRelEntry(relName, relRec, relEntry)))
    return (rc);

  cout << "Total Tuples in Relation: " << relEntry->numTuples << endl;
  cout << endl;
  
  AttrCatEntry *aEntry;
  SM_AttrIterator attrIt;
  RM_Record attrRec;
  if((rc = attrIt.OpenIterator(attrcatFH, relEntry->relName)))
    return (rc);

  for(int i=0; i < relEntry->attrCount; i++){
    if((rc = attrIt.GetNextAttr(attrRec, aEntry))){
      return (rc);
    }
    //int slot = aEntry->attrNum; // insert its info in the appropriate slot

    cout << "  Attribute: " << aEntry->attrName << endl;
    cout << "    Num attributes: " << aEntry->numDistinct << endl;
    cout << "    Max value: " << aEntry->maxValue << endl;
    cout << "    Min value: " << aEntry->minValue << endl;
  }
  if((rc = attrIt.CloseIterator()))
    return (rc);

  return (0);
}

RC SM_Manager::CalcStats(const char *relName){
  RC rc = 0;
  cout << "Calculating stats for relation " << relName << endl;
  if(strlen(relName) > MAXNAME) // check for whether this is a valid name
    return (SM_BADRELNAME);

  // Retrieve the record associated with the relation
  RM_Record relRec;
  RelCatEntry *relEntry;
  if((rc = GetRelEntry(relName, relRec, relEntry)))
    return (rc);

  // Creates a struct containing info about the attributes to 
  // help with loading
  Attr* attributes = (Attr *)malloc(sizeof(Attr)*relEntry->attrCount);
  for(int i=0; i < relEntry->attrCount; i++){
    memset((void*)&attributes[i], 0, sizeof(attributes[i]));
    IX_IndexHandle ih;
    attributes[i] = (Attr) {0, 0, 0, 0, ih, recInsert_string, 0, FLT_MIN, FLT_MAX};
  }
  if((rc = PrepareAttr(relEntry, attributes)))
    return (rc);

  vector<set<string> > numDistinct(relEntry->attrCount);
  for(int i=0; i < relEntry->attrCount; i++){
    attributes[i].numDistinct = 0;
    attributes[i].maxValue = FLT_MIN;
    attributes[i].minValue = FLT_MAX;
  }
  relEntry->numTuples = 0;
  relEntry->statsInitialized = true;

  // Open the relation and iterate through it
  RM_FileScan fs;
  RM_FileHandle fh;
  RM_Record rec;
  if((rc = rmm.OpenFile(relName, fh)) || (rc = fs.OpenScan(fh, INT, 0, 0, NO_OP, NULL)))
    return (rc);
  while(RM_EOF != fs.GetNextRec(rec)){
    char * recData;
    if((rc = rec.GetData(recData)))
      return (rc);

    for(int i = 0;  i < relEntry->attrCount; i++){
      int offset = attributes[i].offset;
      string attr(recData + offset, recData + offset + attributes[i].length);
      numDistinct[i].insert(attr);
      float attrValue = 0.0;
      if(attributes[i].type == STRING)
        attrValue = ConvertStrToFloat(recData + offset);
      else if(attributes[i].type == INT)
        attrValue = (float) *((int*) (recData + offset));
      else
        attrValue = *((float*)(recData + offset));
      if(attrValue > attributes[i].maxValue)
        attributes[i].maxValue = attrValue;
      if(attrValue < attributes[i].minValue)
        attributes[i].minValue = attrValue;
    }
    relEntry->numTuples++;

  }

  // write everything back
  if((rc = relcatFH.UpdateRec(relRec)) || (rc = relcatFH.ForcePages()))
    return (rc);

  SM_AttrIterator attrIt;
  if((rc = attrIt.OpenIterator(attrcatFH, relEntry->relName)))
    return (rc);
  RM_Record attrRec;
  AttrCatEntry *aEntry;
  for(int i = 0; i < relEntry->attrCount; i++){
    if((rc = attrIt.GetNextAttr(attrRec, aEntry)))
      return (rc);
    // For each attribute, place its information in the appropriate slot
    int slot = aEntry->attrNum;
    aEntry->minValue = attributes[slot].minValue;
    aEntry->maxValue = attributes[slot].maxValue;
    aEntry->numDistinct = numDistinct[slot].size();
    if((rc = attrcatFH.UpdateRec(attrRec)))
      return (rc);
  }
  if((rc = attrIt.CloseIterator()))
    return (rc);
  if((rc = attrcatFH.ForcePages()))
    return (rc);


  return (0);
}

