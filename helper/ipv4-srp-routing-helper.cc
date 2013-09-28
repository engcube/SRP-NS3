/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#include "ipv4-srp-routing-helper.h"
#include "ns3/log.h"


NS_LOG_COMPONENT_DEFINE ("SRPRoutingHelper");

namespace ns3 {

void Ipv4SRPRoutingHelper::setCoreNum(int num){
    m_CoreNum = num;
}
void Ipv4SRPRoutingHelper::setToRNum(int num){
    m_ToRNum = num;
}
void Ipv4SRPRoutingHelper::setBorderNum(int num){
    m_BorderNum = num;
}
void Ipv4SRPRoutingHelper::setSubnetMask(int mask){
    m_SubnetMask = mask;
}
void Ipv4SRPRoutingHelper::setAddressStart(uint32_t address){
    m_AddressStart = address;
}
int Ipv4SRPRoutingHelper::getCoreNum(){
    return m_CoreNum;
}
int Ipv4SRPRoutingHelper::getToRNum(){
    return m_ToRNum;
}
int Ipv4SRPRoutingHelper::getBorderNum(){
    return m_BorderNum;
}
int Ipv4SRPRoutingHelper::getSubnetMask(){
    return m_SubnetMask;
}
uint32_t Ipv4SRPRoutingHelper::getAddressStart(){
    return m_AddressStart;
}

map<int, Subnet> Ipv4SRPRoutingHelper::getIndexSubnetMap() const{
  return index_subnet_map;
}

void Ipv4SRPRoutingHelper::addItem2IndexSubnetMap(int index, Subnet subnet){
  index_subnet_map[index] = subnet;
}

Ipv4SRPRoutingHelper::Ipv4SRPRoutingHelper ()
{
}

Ipv4SRPRoutingHelper::Ipv4SRPRoutingHelper (const Ipv4SRPRoutingHelper &o)
{
}

Ipv4SRPRoutingHelper*
Ipv4SRPRoutingHelper::Copy (void) const
{
  return new Ipv4SRPRoutingHelper (*this);
}

void Ipv4SRPRoutingHelper::CreateSRPGrid(Ptr<Node> node) const{

  int id = node->GetId();
  NS_LOG_LOGIC ("Adding SRPGrid to node " << id);
  Ptr<SRPGrid> mSRPGrid = CreateObject<SRPGrid> ();

  NodeType type;
  if( id < m_CoreNum ){
      type = CORE;
  }else if( id < m_ToRNum){
      type = TOR;
  }else {
      type = BORDER;
  }

  if(type == CORE){
      for(int i = m_CoreNum; i< m_CoreNum+m_ToRNum; i++){
          map<int, int> mmap;
          mmap[i] = 1;
          SRPRoutingEntry entry(getIndexSubnetMap()[i], mmap);
          mSRPGrid->addSRPGridEntry(entry);
      }
      map<int, int> mmap;
      for(int i = m_CoreNum+m_ToRNum; i<m_CoreNum+m_ToRNum+m_BorderNum; i++){
          mmap[i] = 1;
      }
      Subnet subnet(0,0);
      SRPRoutingEntry entry(subnet, mmap);
      entry.setDescription("B_exit");
      mSRPGrid->addSRPGridEntry(entry);
  }else if(type == TOR){
      for(int i= m_CoreNum; i< m_CoreNum+m_ToRNum; i++){
         if(i==id){
            continue;
         }
         map<int, int> mmap;
         for(int j=0; j < m_CoreNum; j++){
            mmap[j] = 1;
         }
         SRPRoutingEntry entry(getIndexSubnetMap()[i], mmap);
         mSRPGrid->addSRPGridEntry(entry);
      }
      map<int, int> mmap;
      for(int j=0; j < m_CoreNum; j++){
        mmap[j] = 1;
      }
      Subnet subnet(0,0);
      SRPRoutingEntry entry(subnet, mmap);
      entry.setDescription("B_exit");
      mSRPGrid->addSRPGridEntry(entry);

  }else{ //NodeType.BORDER
      for(int i= m_CoreNum; i< m_CoreNum+m_ToRNum; i++){
         map<int, int> mmap;
         for(int j=0; j < m_CoreNum; j++){
            mmap[j] = 1;
         }
         for(int j=m_CoreNum+m_ToRNum; j<m_CoreNum+m_ToRNum+m_BorderNum; j++){
            if(j==id){
              continue;
            }
            mmap[j] = 3;
         }
         SRPRoutingEntry entry(getIndexSubnetMap()[i], mmap);
         mSRPGrid->addSRPGridEntry(entry);
      }
  }

  node->GetObject<SRPRouter>()->SetSRPGrid (mSRPGrid);
}

Ptr<Ipv4RoutingProtocol>
Ipv4SRPRoutingHelper::Create (Ptr<Node> node) const
{
  int id = node->GetId();


  NS_LOG_LOGIC ("Adding SRPRouter interface to node " <<
                id);
  Ptr<SRPRouter> srpRouter = CreateObject<SRPRouter> ();
  node->AggregateObject (srpRouter);
  
  CreateSRPGrid(node);
  
  //mSRPGlobalInfo.
  NS_LOG_LOGIC ("Adding SRPRouting Protocol to node " << id);
  Ptr<Ipv4SRPRouting> srpRouting = CreateObject<Ipv4SRPRouting> ();
  srpRouter->SetRoutingProtocol (srpRouting);

  return srpRouting;
}

void 
Ipv4SRPRoutingHelper::PopulateRoutingTables (void)
{

  //SRPRouteManager::BuildSRPRoutingDatabase ();
  //SRPRouteManager::InitializeRoutes ();
}
void 
Ipv4SRPRoutingHelper::RecomputeRoutingTables (void)
{
  //SRPRouteManager::DeleteSRPRoutes ();
  //SRPRouteManager::BuildSRPRoutingDatabase ();
  //SRPRouteManager::InitializeRoutes ();
}

uint32_t
Ipv4SRPRoutingHelper::AllocateRouterId (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  static uint32_t routerId = 0;
  return routerId++;
}


} // namespace ns3
