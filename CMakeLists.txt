cmake_minimum_required(VERSION 3.16)
project(DBMS)

set(CMAKE_CXX_STANDARD 14)
# 遍历项目根目录下所有的 .cpp 文件
file (GLOB_RECURSE SOURCE_FILES *.cpp)


add_executable(DBMS main.cpp
#        PagereplacementAlgorithm/PF_pageReplacement.h PagereplacementAlgorithm/PF_pageReplacement.cpp
#        PFSuper/PF_HashTable.h PFSuper/PF_HashTable.cpp PFSuper/PF_BufferPool.cpp PFSuper/PF_BufferPool.h PagereplacementAlgorithm/LRUPageReplacement.h PagereplacementAlgorithm/LRUPageReplacement.cpp PagereplacementAlgorithm/CLOCKPageReplacement.cpp PagereplacementAlgorithm/CLOCKPageReplacement.h
        PF/pf_error.cc PF/pf_filehandle.cc PF/pf_hashtable.cc PF/pf_manager.cc PF/pf_pagehandle.cc PF/pf_statistics.cc PF/pf_buffermgr.cc
        RM/rm_error.cc RM/rm_filehandle.cc RM/rm_filescan.cc RM/rm_manager.cc RM/rm_record.cc RM/rm_rid.cc
        )

