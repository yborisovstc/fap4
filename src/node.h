
#ifndef __NDEDS_NODE_H
#define __NDEDS_NODE_H

#include <string>

#include "state.h"


class Node
{
    public:
	Node(const std::string& aName);
    public:
	//PState<std::string>::TOcp cpName;
    protected:
	PState<std::string> sName;
};

#endif


