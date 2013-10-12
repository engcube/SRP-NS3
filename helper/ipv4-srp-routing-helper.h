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
#ifndef IPV4_SRP_ROUTING_HELPER_H
#define IPV4_SRP_ROUTING_HELPER_H

#include <map>
#include <iostream>

#include "ns3/srp-router-interface.h"

#include "ns3/node-container.h"
#include "ns3/ipv4-routing-helper.h"
#include "ns3/subnet.h"

#include "ns3/conf-loader.h"
#include "ns3/ipv4-srp-routing.h"
#include "ns3/ipv4-list-routing.h"

using namespace std;

namespace ns3 {

/**
 * \brief Helper class that adds ns3::Ipv4SRPRouting objects
 */
class Ipv4SRPRoutingHelper  : public Ipv4RoutingHelper
{
public:
  /**
   * \brief Construct a SRPRoutingHelper to make life easier for managing
   * SRP routing tasks.
   */
  Ipv4SRPRoutingHelper ();
  Ipv4SRPRoutingHelper (ConfLoader mconf);
  /**
   * \brief Construct a SRPRoutingHelper from another previously initialized
   * instance (Copy Constructor).
   */
  Ipv4SRPRoutingHelper (const Ipv4SRPRoutingHelper &);

  /**
   * \internal
   * \returns pointer to clone of this Ipv4SRPRoutingHelper
   *
   * This method is mainly for internal use by the other helpers;
   * clients are expected to free the dynamic memory allocated by this method
   */
  Ipv4SRPRoutingHelper* Copy (void) const;

  /**
   * \param node the node on which the routing protocol will run
   * \returns a newly-created routing protocol
   *
   * This method will be called by ns3::InternetStackHelper::Install
   */
  virtual Ptr<Ipv4RoutingProtocol> Create (Ptr<Node> node) const;

  /**
   * \brief Build a routing database and initialize the routing tables of
   * the nodes in the simulation.  Makes all nodes in the simulation into
   * routers.
   *
   * All this function does is call the functions
   * BuildSRPRoutingDatabase () and  InitializeRoutes ().
   *
   */
  static void PopulateRoutingTables (void);
  /**
   * \brief Remove all routes that were previously installed in a prior call
   * to either PopulateRoutingTables() or RecomputeRoutingTables(), and 
   * add a new set of routes.
   * 
   * This method does not change the set of nodes
   * over which SRPRouting is being used, but it will dynamically update
   * its representation of the SRP topology before recomputing routes.
   * Users must first call PopulateRoutingTables() and then may subsequently
   * call RecomputeRoutingTables() at any later time in the simulation.
   *
   */
  static void RecomputeRoutingTables (void);

  static uint32_t AllocateRouterId ();

  void CreateSRPGrid(Ptr<Node> node) const;

private:
  /**
   * \internal
   * \brief Assignment operator declared private and not implemented to disallow
   * assignment and prevent the compiler from happily inserting its own.
   */
  Ipv4SRPRoutingHelper &operator = (const Ipv4SRPRoutingHelper &o);



};

} // namespace ns3

#endif /* IPV4_SRP_ROUTING_HELPER_H */
