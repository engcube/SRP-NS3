#ifndef SRP_GLOBAL_INFO_H
#define SRP_GLOBAL_INFO_H

#include <iostream>
#include <map>

#include "ns3/subnet.h"

using namespace std;

namespace ns3{

class SRPGlobalInfo{
public:
	map<int, Subnet> getIndexSubnetMap();
	void addItem2IndexSubnetMap(int index, Subnet subnet);
public:
	map<int, Subnet> index_subnet_map;
};

extern SRPGlobalInfo mSRPGlobalInfo;

}

#endif /*SRP_GLOBAL_INFO_H*/