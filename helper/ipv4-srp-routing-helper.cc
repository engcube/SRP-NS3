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

Ptr<Ipv4RoutingProtocol>
Ipv4SRPRoutingHelper::Create (Ptr<Node> node) const
{
  int id = node->GetId();

  NS_LOG_LOGIC ("Adding SRPRouter interface to node " <<
                id);
  Ptr<SRPRouter> srpRouter = CreateObject<SRPRouter> ();
  node->AggregateObject (srpRouter);
  
  //mSRPGlobalInfo.
  NS_LOG_LOGIC ("Adding SRPRouting Protocol to node " << id);
  Ptr<Ipv4SRPRouting> srpRouting = CreateObject<Ipv4SRPRouting> ();
  srpRouting->setID(id);
  srpRouter->setID(id);
  srpRouter->SetRoutingProtocol (srpRouting);

  Ptr<SRPGrid> mSRPGrid = CreateObject<SRPGrid> ();
  ConfLoader::Instance()->UpdateSRPGrid(node->GetId(), mSRPGrid);
  node->GetObject<SRPRouter>()->GetRoutingProtocol()->SetSRPGrid (mSRPGrid);

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

/*
void Ipv4SRPRoutingHelper::UpdateSRPGrid(int id, Ptr<SRPGrid> mSRPGrid){
  NS_LOG_LOGIC ("Updating SRPGrid to node " << id);
  //Ptr<SRPGrid> mSRPGrid = node->GetObject<SRPRouter>()->GetRoutingProtocol()->GetSRPGrid();
  if(ConfLoader::Instance()->getNodeState(id)){
    
    if( id < ConfLoader::Instance()->getCoreNum() ){  //Core
    
        for(int i = ConfLoader::Instance()->getCoreNum(); i< ConfLoader::Instance()->getCoreNum()+ConfLoader::Instance()->getToRNum(); i++){
            if(ConfLoader::Instance()->getNodeState(i) && ConfLoader::Instance()->getLinkState(id,i)){
                map<int, int> mmap;
                mmap[i] = 1;
                SRPRoutingEntry entry(ConfLoader::Instance()->getIndexSubnetMap()[i], mmap);
                mSRPGrid->addSRPGridEntry(entry);
            } 
        }
        map<int, int> mmap;
        for(int i = ConfLoader::Instance()->getCoreNum()+ConfLoader::Instance()->getToRNum(); i<ConfLoader::Instance()->getTotalNum(); i++){
            if(ConfLoader::Instance()->getNodeState(i)){
              mmap[i] = 1;
            }
        }
        Subnet subnet(0,0);
        SRPRoutingEntry entry(subnet, mmap);
        entry.setDescription("B_exit");
        mSRPGrid->addSRPGridEntry(entry);

    }

    else if( id < ConfLoader::Instance()->getCoreNum()+ConfLoader::Instance()->getToRNum()){ //ToR
        
        for(int i= ConfLoader::Instance()->getCoreNum(); i< ConfLoader::Instance()->getCoreNum()+ConfLoader::Instance()->getToRNum(); i++){
           if(i==id){
              continue;
           }
           map<int, int> mmap;
           for(int j=0; j < ConfLoader::Instance()->getCoreNum(); j++){
            if(ConfLoader::Instance()->getNodeState(j)){
              mmap[j] = 1;
            }
           }
           SRPRoutingEntry entry(ConfLoader::Instance()->getIndexSubnetMap()[i], mmap);
           mSRPGrid->addSRPGridEntry(entry);
        }
        map<int, int> mmap;
        for(int j=0; j < ConfLoader::Instance()->getCoreNum(); j++){
          if(ConfLoader::Instance()->getNodeState(j)){
              mmap[j] = 1;
            }
        }
        Subnet subnet(0,0);
        SRPRoutingEntry entry(subnet, mmap);
        entry.setDescription("B_exit");
        mSRPGrid->addSRPGridEntry(entry);

    }else { //NodeType.BORDER
        for(int i= ConfLoader::Instance()->getCoreNum(); i< ConfLoader::Instance()->getCoreNum()+ConfLoader::Instance()->getToRNum(); i++){
           map<int, int> mmap;
           for(int j=0; j < ConfLoader::Instance()->getCoreNum(); j++){
              if(ConfLoader::Instance()->getNodeState(j)){
                mmap[j] = 1;
              }
           }
           for(int j=ConfLoader::Instance()->getCoreNum()+ConfLoader::Instance()->getToRNum(); j<ConfLoader::Instance()->getTotalNum(); j++){
              if(j==id){
                continue;
              }
              if(ConfLoader::Instance()->getNodeState(j)){
                mmap[j] = 3;
              }
           }
           SRPRoutingEntry entry(ConfLoader::Instance()->getIndexSubnetMap()[i], mmap);
           mSRPGrid->addSRPGridEntry(entry);
        }
    }
  }
}
*/

} // namespace ns3
