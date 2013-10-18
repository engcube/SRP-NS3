#ifndef CONF_LOADER_H
#define CONF_LOADER_H

#include <string>
#include <iostream>
#include <stdint.h>
#include <map>

#include "ns3/node-container.h"
#include "ns3/subnet.h"
using namespace std;

namespace ns3{

class NodeContainer;

class ConfLoader
{
public:
  static ConfLoader* Instance();
  void setCoreNum(int num);
  void setToRNum(int num);
  void setBorderNum(int num);
  void setSubnetMask(int mask);
  void setAddressStart(uint32_t address);
  int getCoreNum() const;
  int getToRNum() const;
  int getBorderNum() const;
  int getSubnetMask() const;
  int getTotalNum() const;

  uint32_t getAddressStart() const;


  map<int, Subnet> getIndexSubnetMap() const;
  void addItem2IndexSubnetMap(int index, Subnet subnet);

  void setNodeContainer(NodeContainer nc);
  NodeContainer getNodeContainer();
  int getInterfaceIndex(int my, int to);

  map<int, bool> getNodeStates();
  void setNodeStates(map<int, bool> states);

  map<pair<int,int>,bool> getLinkStates();
  void setLinkStates(map<pair<int,int>,bool> states);


  map<int, bool> getNodeActions();
  void setNodeActions(map<int, bool> actions);

  map<pair<int,int>,bool> getLinkActions();
  void setLinkActions(map<pair<int,int>,bool> actions);

  void clearNodeActions();
  void clearLinkActions();

private:
	ConfLoader(){};
	ConfLoader(ConfLoader const&){};
	//ConfLoader& operator=(ConfLoader const&){};
	static ConfLoader* m_pInstance;

  enum NodeType{
      CORE = 0,
      TOR,
      BORDER,
  };

  map<int, Subnet> index_subnet_map;

  map<int, bool> nodeStates;
  map<pair<int,int>,bool> linkStates;

  map<int, bool> nodeActions;
  map<pair<int,int>,bool> linkActions;

  int m_CoreNum;
  int m_ToRNum;
  int m_BorderNum;
  int m_SubnetMask;
  uint32_t m_AddressStart;

  NodeContainer m_nodes;
};

}

#endif /* CONF_LOADER_H */