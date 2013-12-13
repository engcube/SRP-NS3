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

  //NS_LOG_LOGIC ("Adding SRPRouter interface to node " << id);

  //Ptr<SRPRouter> router = CreateObject<SRPRouter> ();
  //node->AggregateObject (router);
  
  //mSRPGlobalInfo.
  NS_LOG_LOGIC ("Adding SRPRouting Protocol to node " << id);
  Ptr<Ipv4SRPRouting> routing = CreateObject<Ipv4SRPRouting> ();
  routing->setID(id);
  routing->initGrid();
  //router->setID(id);
  //router->SetRoutingProtocol (routing);
  map<Subnet, vector<int> > mSRPRoutingTable;
  //Subnet subnet(0x0a000000, 24);
  //mSRPRoutingTable.insert(pair<Subnet, int>(subnet, id));
  //mSRPRoutingTable[subnet] = id;
  //ConfLoader::Instance()->update(id, mSRPRoutingTable);
  routing->SetSRPRoutingTable(mSRPRoutingTable);
  //routing->GetSRPRoutingTable()[subnet] = id;
  //ConfLoader::Instance()->UpdateSRPGrid(node->GetId(), mSRPRoutingTable);
  //node->GetObject<SRPRouter>()->GetRoutingProtocol()->SetSRPRoutingTable (mSRPRoutingTable);
  node->AggregateObject(routing);
  cout << "Create Helper" << endl;
  return routing;
}



void 
Ipv4SRPRoutingHelper::PopulateRoutingTables (void)
{
}
void 
Ipv4SRPRoutingHelper::RecomputeRoutingTables (void)
{
}

uint32_t
Ipv4SRPRoutingHelper::AllocateRouterId (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  static uint32_t routerId = 0;
  return routerId++;
}

} // namespace ns3
