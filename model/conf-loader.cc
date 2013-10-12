#include <fstream>
#include <iostream>

#include "conf-loader.h"
//#include "ns3/log.h"


//NS_LOG_COMPONENT_DEFINE ("ConfLoader");

namespace ns3{

using namespace std;

ConfLoader* ConfLoader::m_pInstance = NULL;

ConfLoader* ConfLoader::Instance(){
	if(!m_pInstance){
		m_pInstance = new ConfLoader;
	}
	return m_pInstance;
}

void ConfLoader::setCoreNum(int num){
    m_CoreNum = num;
    cout << "Core " << m_CoreNum << endl;
}
void ConfLoader::setToRNum(int num){
    m_ToRNum = num;
}
void ConfLoader::setBorderNum(int num){
    m_BorderNum = num;
}
void ConfLoader::setSubnetMask(int mask){
    m_SubnetMask = mask;
}
void ConfLoader::setAddressStart(uint32_t address){
    m_AddressStart = address;
}
int ConfLoader::getCoreNum() const{
    return m_CoreNum;
}
int ConfLoader::getToRNum() const{
    return m_ToRNum;
}
int ConfLoader::getBorderNum() const{
    return m_BorderNum;
}
int ConfLoader::getSubnetMask() const{
    return m_SubnetMask;
}
uint32_t ConfLoader::getAddressStart() const{
    return m_AddressStart;
}

map<int, Subnet> ConfLoader::getIndexSubnetMap() const{
  return index_subnet_map;
}

void ConfLoader::addItem2IndexSubnetMap(int index, Subnet subnet){
  index_subnet_map[index] = subnet;
}


}