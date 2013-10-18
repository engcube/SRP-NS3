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
 * Authors:  Craig Dowell (craigdo@ee.washington.edu)
 *           Tom Henderson (tomhend@u.washington.edu)
 */

#ifndef SRP_ROUTER_INTERFACE_H
#define SRP_ROUTER_INTERFACE_H

#include <stdint.h>
#include <list>
#include <string>
#include <sstream>

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/channel.h"
#include "ns3/ipv4-address.h"
#include "ns3/net-device-container.h"
#include "ns3/bridge-net-device.h"
//#include "ns3/srp-route-manager.h"
#include "ns3/ipv4-routing-table-entry.h"

#include "ns3/subnet.h"
#include "ns3/ipv4-srp-routing-helper.h"

using namespace std;

namespace ns3 {

class SRPRoutingEntry;
class SRPGrid;
class SRPRouter;
class Ipv4SRPRouting;
class Ipv4Address;

class SRPRoutingEntry{
public:
    SRPRoutingEntry();
    SRPRoutingEntry(Subnet subnet, map<int, int> nodeList);
    void removeNode(int index);
    void addNode(int index, int status);
    Subnet getSubnet();
    string getDescription();
    void setDescription(string desc);
    map<int, int> getNodeList();
    void setNodeList(map<int, int> mNodeList); 
    string toString();
private:
    Subnet mSubnet;
    map<int, int> mNodeList; 
    string m_description;
};

class SRPGrid : public Object{
public:
    enum GridType{
        CORE = 0,
        TOR,
        BORDER,
    };
    friend class SRPRoutingEntry;
    static TypeId GetTypeId (void);
    SRPGrid();
    void addSRPGridEntry(SRPRoutingEntry entry);
    void removeSRPGridEntry(Subnet subnet);
    list<SRPRoutingEntry> getSRPGrid();
    void setType(GridType type);
    string toString();
    map<int, int> getNodeListByHost(Ipv4Address dest);
private:
    GridType m_type;
    list<SRPRoutingEntry> m_entries;
};

/**
 * @brief An interface aggregated to a node to provide SRP routing info
 *
 * An interface aggregated to a node that provides SRP routing information
 * to a SRP route manager.  The presence of the interface indicates that
 * the node is a router.  The interface is the mechanism by which the router
 * advertises its connections to neighboring routers.  We're basically 
 * allowing the route manager to query for link state advertisements.
 */
class SRPRouter : public Object
{
public:
/**
 * @brief The Interface ID of the SRP Router interface.
 *
 * @see Object::GetObject ()
 */
  static TypeId GetTypeId (void);

/**
 * @brief Create a SRP Router class 
 */
  SRPRouter ();


  void SetRoutingProtocol (Ptr<Ipv4SRPRouting> routing);
  Ptr<Ipv4SRPRouting> GetRoutingProtocol (void);

/**
 * @brief Get the Router ID associated with this SRP Router.
 *
 * The Router IDs are allocated in the RoutingEnvironment -- one per Router, 
 * starting at 0.0.0.1 and incrementing with each instantiation of a router.
 *
 * @see RoutingEnvironment::AllocateRouterId ()
 * @returns The Router ID associated with the SRP Router.
 */
  Ipv4Address GetRouterId (void) const;


  int getID(){return m_id;};
  void setID(int index){m_id = index;};
private:
  int m_id;
  virtual ~SRPRouter ();
  
  Ipv4Address m_routerId;
  Ptr<Ipv4SRPRouting> m_routingProtocol;

  // inherited from Object
  virtual void DoDispose (void);

/**
 * @brief SRP Router copy construction is disallowed.
 */
  SRPRouter (SRPRouter& sr);

/**
 * @brief SRP Router assignment operator is disallowed.
 */
  SRPRouter& operator= (SRPRouter& sr);
};

} // namespace ns3

#endif /* SRP_ROUTER_INTERFACE_H */
