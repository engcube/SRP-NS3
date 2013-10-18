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

int ConfLoader::getTotalNum() const{
    return m_CoreNum+m_ToRNum+m_BorderNum;
}


map<int, Subnet> ConfLoader::getIndexSubnetMap() const{
  return index_subnet_map;
}

void ConfLoader::addItem2IndexSubnetMap(int index, Subnet subnet){
  index_subnet_map[index] = subnet;
}

void ConfLoader::setNodeContainer(NodeContainer nc){
    m_nodes = nc;
}

NodeContainer ConfLoader::getNodeContainer(){
    return m_nodes;
}

int ConfLoader::getInterfaceIndex(int my, int to){
    if(my<m_CoreNum){
        if(to<m_CoreNum){
            return 0;
        }else{
            return to-m_CoreNum+1;
        }
    }else{
        if(to>=m_CoreNum){
            return 0;
        }else{
            return to+1;
        }
    }
    return 0;
}

map<int, bool> ConfLoader::getNodeStates(){
    return this->nodeStates;
}

void ConfLoader::setNodeStates(map<int, bool> states){
    this->nodeStates = states;
}

map<pair<int,int>,bool> ConfLoader::getLinkStates(){
    return this->linkStates;
}

void ConfLoader::setLinkStates(map<pair<int,int>,bool> states){
    this->linkStates = states;
}

map<int, bool> ConfLoader::getNodeActions(){
    return this->nodeActions;
}

void ConfLoader::setNodeActions(map<int, bool> actions){
    this->nodeActions = actions;
}

map<pair<int,int>,bool> ConfLoader::getLinkActions(){
    return this->linkActions;
}

void ConfLoader::setLinkActions(map<pair<int,int>,bool> actions){
    this->linkActions = actions;
}

void ConfLoader::clearNodeActions(){
    this->nodeActions.clear();
}

void ConfLoader::clearLinkActions(){
    this->linkActions.clear();
}


}