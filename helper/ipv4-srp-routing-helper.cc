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
#include "ns3/srp-router-interface.h"
#include "ns3/ipv4-srp-routing.h"
#include "ns3/ipv4-list-routing.h"
#include "ns3/log.h"

//#include <iostream>
//using namespace std;

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
  NS_LOG_LOGIC ("Adding SRPRouter interface to node " <<
                node->GetId ());

  Ptr<SRPRouter> srpRouter = CreateObject<SRPRouter> ();
  node->AggregateObject (srpRouter);

  NS_LOG_LOGIC ("Adding SRPRouting Protocol to node " << node->GetId ());
  Ptr<Ipv4SRPRouting> srpRouting = CreateObject<Ipv4SRPRouting> ();
  srpRouter->SetRoutingProtocol (srpRouting);

  return srpRouting;
}

void 
Ipv4SRPRoutingHelper::PopulateRoutingTables (void)
{
  SRPRouteManager::BuildSRPRoutingDatabase ();
  SRPRouteManager::InitializeRoutes ();
}
void 
Ipv4SRPRoutingHelper::RecomputeRoutingTables (void)
{
  SRPRouteManager::DeleteSRPRoutes ();
  SRPRouteManager::BuildSRPRoutingDatabase ();
  SRPRouteManager::InitializeRoutes ();
}


} // namespace ns3
