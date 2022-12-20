// 
// File:          node.cc
// Description:   Abstract class for query processing nodes
// Author:        Yifei Huang (yifei@stanford.edu)
//

#include <cstdio>
#include <iostream>
#include <unistd.h>
#include "../redbase.h"
#include "../SM/sm.h"
#include "../RM/rm.h"
#include "ql.h"
#include "../IX/ix.h"
#include <string>
#include "ql_node.h"

using namespace std;

QL_Node::QL_Node(){

}

QL_Node::~QL_Node(){

}

