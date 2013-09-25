#include "ns3/srp-global-info.h"

namespace ns3{

SRPGlobalInfo mSRPGlobalInfo;

map<int, Subnet> SRPGlobalInfo::getIndexSubnetMap(){
	return index_subnet_map;
}

void SRPGlobalInfo::addItem2IndexSubnetMap(int index, Subnet subnet){
	index_subnet_map[index] = subnet;
}

}