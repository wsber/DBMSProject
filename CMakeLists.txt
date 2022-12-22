cmake_minimum_required(VERSION 3.16)
project(DBMS)

set(CMAKE_CXX_STANDARD 14)
# 遍历项目根目录下所有的 .cpp 文件
file (GLOB_RECURSE SOURCE_FILES *.cpp)


add_executable(DBMS main.cpp
         PF/pf_error.cc PF/pf_filehandle.cc PF/pf_hashtable.cc PF/pf_manager.cc PF/pf_pagehandle.cc PF/pf_statistics.cc PF/pf_buffermgr.cc
         RM/rm_error.cc RM/rm_filehandle.cc RM/rm_filescan.cc RM/rm_manager.cc RM/rm_record.cc RM/rm_rid.cc
        IX/ix_error.cc IX/ix_indexhandle.cc IX/ix_indexscan.cc IX/ix_manager.cc
#        QL/interp.c QL/parse.cc QL/parser_test.cc QL/ql_error.cc QL/ql_manager.cc QL/ql_manager_stub.cc QL/ql_node.cc QL/ql_nodejoin.cc QL/ql_nodeproj.cc QL/ql_noderel.cc QL/ql_nodesel.cc QL/qo_manager.cc QL/scan.c QL/scanhelp.c
#        SM/sm_attriterator.cc SM/sm_error.cc SM/sm_stub.cc SM/sm_manager.cc
        statistics.cc printer.cc
#        QL/parser.h PF/pf.h
#        QL/node.cc QL/node_proj.cc QL/node_rel.cc QL/node_sel.cc QL/nodes.c
        )
