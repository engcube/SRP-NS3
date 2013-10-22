/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2007 University of Washington
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:  Tom Henderson (tomhend@u.washington.edu)
 */

#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/abort.h"
#include "ns3/channel.h"
#include "ns3/net-device.h"
#include "ns3/node.h"
#include "ns3/node-list.h"
#include "ns3/ipv4.h"
#include "ns3/bridge-net-device.h"
#include "ipv4-srp-routing.h"
#include "srp-router-interface.h"
#include <vector>

NS_LOG_COMPONENT_DEFINE ("SRPRouter");

namespace ns3 {

// ---------------------------------------------------------------------------
//
// SRPRoutingLinkRecord Implementation
//
// ---------------------------------------------------------------------------
TypeId SRPGrid::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SRPGrid")
    .SetParent<Object> ();
  return tid;
}

SRPRoutingEntry::SRPRoutingEntry(){

}

SRPRoutingEntry::SRPRoutingEntry(Subnet subnet, map<int, int> nodeList){
    this->mSubnet = subnet;
    this->mNodeList = nodeList;
}

void SRPRoutingEntry::removeNode(int index){
    this->mNodeList.erase(index);
}

void SRPRoutingEntry::addNode(int index, int status){
    this->mNodeList[index] = status;
}

Subnet SRPRoutingEntry::getSubnet(){
    return this->mSubnet;
}

map<int, int> SRPRoutingEntry::getNodeList(){
    return this->mNodeList;
}

void SRPRoutingEntry::setNodeList(map<int, int> nodeList){
    this->mNodeList = nodeList;
}

SRPGrid::SRPGrid(){

}

string SRPRoutingEntry::toString(){
    stringstream ss;
    ss << mSubnet.toString() << ":" << m_description << endl;
    for(map<int, int>::iterator it = mNodeList.begin(); it!=mNodeList.end();++it){
        ss << it->first << ":" << it->second << "\t";
    }
    return ss.str();
}

string SRPGrid::toString(){
    stringstream ss;
    for(list<SRPRoutingEntry>::iterator it = m_entries.begin(); it != m_entries.end(); ++it){
        ss << it->toString() << endl;
    }
    return ss.str();
}
void SRPGrid::setType(GridType type){
    m_type = type;
}

string SRPRoutingEntry::getDescription(){
    return m_description;
}

void SRPRoutingEntry::setDescription(string desc){
    m_description = desc;
}

bool SRPRoutingEntry::hasEffectSubnet(){
    for(map<int, int>::iterator it = mNodeList.begin(); it != mNodeList.end(); ++it){
        if(it->second==1 || it->second==3){
          return true;
        }
    }
    return false;
}

vector<Subnet> SRPGrid::getEffectSubnet(){
  vector<Subnet> effcetSubnets;
  for(list<SRPRoutingEntry>::iterator it = m_entries.begin(); it != m_entries.end(); ++it){
    if(it->hasEffectSubnet()){
      effcetSubnets.push_back(it->getSubnet());
    }
  }
  return effcetSubnets;
}

void SRPGrid::addSRPGridEntry(SRPRoutingEntry entry){
    m_entries.push_back(entry);
}

void SRPGrid::removeSRPGridEntry(Subnet subnet){
    for(list<SRPRoutingEntry>::iterator it=m_entries.begin(); it != m_entries.end(); ++it){
        if(it->getSubnet().equals(subnet)){
            m_entries.erase(it);
            break;
        }
    }
}

list<SRPRoutingEntry> SRPGrid::getSRPGrid(){
    return m_entries;
}

map<int, int> SRPGrid::getNodeListByHost(Ipv4Address dest){
    for(list<SRPRoutingEntry>::iterator it=m_entries.begin(); it != m_entries.end(); ++it){
        if(it->getSubnet().contains(dest)){
            return it->getNodeList();
        }
    }
    map<int,int> emp;
    return emp;
}


// ---------------------------------------------------------------------------
//
// SRPRouter Implementation
//
// ---------------------------------------------------------------------------

NS_OBJECT_ENSURE_REGISTERED (SRPRouter);

TypeId 
SRPRouter::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SRPRouter")
    .SetParent<Object> ();
  return tid;
}

SRPRouter::SRPRouter ()
{
  NS_LOG_FUNCTION (this);
  m_routerId.Set (Ipv4SRPRoutingHelper::AllocateRouterId ());
  //Time onInterval = Seconds (1.02);
  //Simulator::Schedule (onInterval, &update);

}

SRPRouter::~SRPRouter ()
{
  NS_LOG_FUNCTION (this);
}

/*void SRPRouter::updateNode(){
  cout << "update Node" << m_id << endl;
  if (ConfLoader::Instance()->getNodeActions().count(m_id)>0){
      if(ConfLoader::Instance()->getNodeAction(m_id) && ConfLoader::Instance()->getNodeState(m_id)==false){
          //send UP message
          ConfLoader::Instance()->setNodeState(m_id, true);
      }else if(ConfLoader::Instance()->getNodeAction(m_id) && ConfLoader::Instance()->getNodeState(m_id)==true){
          //send DOWN message
          ConfLoader::Instance()->setNodeState(m_id, false);
      }
  }
}

void SRPRouter::updateLink(){
  cout << "update Link " << m_id << endl;
  if(m_id < ConfLoader::Instance()->getCoreNum()){ //Core
    for(int i=ConfLoader::Instance()->getCoreNum(); i<ConfLoader::Instance()->getTotalNum();i++){
      if (ConfLoader::Instance()->getNodeState(i)&&ConfLoader::Instance()->getLinkState(m_id,j))
      {
      }
    }

  }else if(m_id < ConfLoader::Instance()->getToRNum()){ //ToR

  }else{ //Border

  }
}
*/

bool SRPRouter::update(){
  cout << "update  " << m_id << endl;
  /*if(m_id < ConfLoader::Instance()->getCoreNum()){ //Core
    for(int i=ConfLoader::Instance()->getCoreNum(); i<ConfLoader::Instance()->getTotalNum();i++){
      if (ConfLoader::Instance()->getNodeState(i)&&ConfLoader::Instance()->getLinkState(m_id,i))
      {
      }
    }
  }else if(m_id < ConfLoader::Instance()->getToRNum()){ //ToR

  }else{ //Border

  }*/
  Ptr<SRPGrid> mGrid = CreateObject<SRPGrid> ();
  ConfLoader::Instance()->UpdateSRPGrid(m_id, mGrid);
  vector<Subnet> lastList = m_routingProtocol->GetSRPGrid()->getEffectSubnet();
  vector<Subnet> curList = mGrid->getEffectSubnet();
  m_routingProtocol->SetSRPGrid(mGrid);
  return true;
}

void 
SRPRouter::SetRoutingProtocol (Ptr<Ipv4SRPRouting> routing)
{
  NS_LOG_FUNCTION (this << routing);
  m_routingProtocol = routing;
}
Ptr<Ipv4SRPRouting> 
SRPRouter::GetRoutingProtocol (void)
{
  NS_LOG_FUNCTION (this);
  return m_routingProtocol;
}

void
SRPRouter::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_routingProtocol = 0;
  Object::DoDispose ();
}

Ipv4Address
SRPRouter::GetRouterId (void) const
{
  NS_LOG_FUNCTION (this);
  return m_routerId;
}


} // namespace ns3
