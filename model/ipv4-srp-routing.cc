// -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*-
//
// Copyright (c) 2008 University of Washington
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation;
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include <vector>
#include <iomanip>
#include "ns3/names.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/net-device.h"
#include "ns3/ipv4-route.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/boolean.h"
#include "ipv4-srp-routing.h"

NS_LOG_COMPONENT_DEFINE ("Ipv4SRPRouting");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Ipv4SRPRouting);

TypeId 
Ipv4SRPRouting::GetTypeId (void)
{ 
  static TypeId tid = TypeId ("ns3::Ipv4SRPRouting")
    .SetParent<Object> ()
    .AddAttribute ("RandomEcmpRouting",
                   "Set to true if packets are randomly routed among ECMP; set to false for using only one route consistently",
                   BooleanValue (false),
                   MakeBooleanAccessor (&Ipv4SRPRouting::m_randomEcmpRouting),
                   MakeBooleanChecker ())
    .AddAttribute ("RespondToInterfaceEvents",
                   "Set to true if you want to dynamically recompute the SRP routes upon Interface notification events (up/down, or add/remove address)",
                   BooleanValue (false),
                   MakeBooleanAccessor (&Ipv4SRPRouting::m_respondToInterfaceEvents),
                   MakeBooleanChecker ())
  ;
  return tid;
}

Ipv4SRPRouting::Ipv4SRPRouting () 
  : m_randomEcmpRouting (false),
    m_respondToInterfaceEvents (false)
{
  NS_LOG_FUNCTION (this);
  m_rand = CreateObject<UniformRandomVariable> ();
}

Ipv4SRPRouting::~Ipv4SRPRouting ()
{
  NS_LOG_FUNCTION (this);
}

void Ipv4SRPRouting::SetSRPGrid(Ptr<SRPGrid> grid){
    this->m_SRPGrid = grid;
}

Ptr<SRPGrid> Ipv4SRPRouting::GetSRPGrid (void){
    return this->m_SRPGrid;
}

void
Ipv4SRPRouting::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Ipv4RoutingProtocol::DoDispose ();
}

// Formatted like output of "route -n" command
void
Ipv4SRPRouting::PrintRoutingTable (Ptr<OutputStreamWrapper> stream) const
{
  NS_LOG_FUNCTION (this << stream);
}

/*Ptr<SRPRouter> Ipv4SRPRouting::getRouter() const{
  return m_router;
}

void Ipv4SRPRouting::setRouter(Ptr<SRPRouter> router){
  m_router = router;
}
*/

Ptr<Ipv4Route> Ipv4SRPRouting::LookupSRPGrid (Ipv4Address dest)
{
  NS_LOG_LOGIC ("Looking for route for destination " << dest);
  map<int, int> nodeList = m_SRPGrid->getNodeListByHost(dest);
  //value = 1
  vector<int> v1_list;
  //value = 3
  vector<int> v3_list;
  for(map<int, int>::iterator it = nodeList.begin(); it != nodeList.end(); ++it){
      if(it->second==1){
          v1_list.push_back(it->first);
      }else if(it->second==3){
          v3_list.push_back(it->first);
      }
  }

  //routing algorthm: how to choose a path
  // pick up one of the routes uniformly at random if random
  // ECMP routing is enabled, or always select the first route
  // consistently if random ECMP routing is disabled

  int destNode = -1;
  if(v1_list.size()>0){
      if (m_randomEcmpRouting){
          destNode = v1_list[m_rand->GetInteger (0, v1_list.size ()-1)];
      }
      else{
          destNode = v1_list.front();
      }

  }else if(v3_list.size()>0){
      if (m_randomEcmpRouting){
          destNode = v3_list[m_rand->GetInteger (0, v3_list.size ()-1)];
      }
      else{
          destNode = v3_list.front();
      }
  }else{
      return 0;
  }

  NS_LOG_LOGIC ("Route Found to Node " << destNode);
  int to_index_of_interface = ConfLoader::Instance()->getInterfaceIndex(destNode, m_id);
  int my_index_of_interface = ConfLoader::Instance()->getInterfaceIndex(m_id, destNode);

  cout << "Route from this node "<<m_id <<" on interface " << my_index_of_interface <<" to Node " << destNode << " on interface " << to_index_of_interface << endl;

  Ptr<Ipv4> to_ipv4 = ConfLoader::Instance()->getNodeContainer().Get(destNode)->GetObject<SRPRouter>()->GetRoutingProtocol()->getIpv4();

  Ptr<Ipv4Route> rtentry = Create<Ipv4Route> ();
  rtentry->SetDestination (to_ipv4->GetAddress (to_index_of_interface, 0).GetLocal ());

  // XXX handle multi-address case
  rtentry->SetSource (m_ipv4->GetAddress (my_index_of_interface, 0).GetLocal ());

  rtentry->SetGateway (Ipv4Address("0.0.0.0"));
  //uint32_t interfaceIdx = route->GetInterface ();
  //rtentry->SetOutputDevice (m_ipv4->GetNetDevice (interfaceIdx));
  rtentry->SetOutputDevice (m_ipv4->GetNetDevice (my_index_of_interface));
  //Ptr<Ipv4Route> rtentry = 0;
  return rtentry;
}


Ptr<Ipv4Route>
Ipv4SRPRouting::RouteOutput (Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr)
{
  NS_LOG_FUNCTION (this << p << &header << oif << &sockerr);
//
// First, see if this is a multicast packet we have a route for.  If we
// have a route, then send the packet down each of the specified interfaces.
//
  cout << m_id <<" send a packet\t"<<header.GetSource() << "\t"<<header.GetDestination()<<endl;

  if (header.GetDestination ().IsMulticast ())
    {
      NS_LOG_LOGIC ("Multicast destination-- returning false");
      return 0; // Let other routing protocols try to handle this
    }
//
// See if this is a unicast packet we have a route for.
//
  NS_LOG_LOGIC ("Unicast destination- looking up");
  Ptr<Ipv4Route> rtentry = LookupSRPGrid (header.GetDestination ());
  if (rtentry)
    {
      sockerr = Socket::ERROR_NOTERROR;
    }
  else
    {
      sockerr = Socket::ERROR_NOROUTETOHOST;
    }
  return rtentry;
}

Ptr<Ipv4> Ipv4SRPRouting::getIpv4(){
  return m_ipv4;
}

bool 
Ipv4SRPRouting::RouteInput  (Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev,                             
                UnicastForwardCallback ucb, MulticastForwardCallback mcb,
                                LocalDeliverCallback lcb, ErrorCallback ecb)
{ 
  NS_LOG_FUNCTION (this << p << header << header.GetSource () << header.GetDestination () << idev << &lcb << &ecb);
  // Check if input device supports IP
  NS_ASSERT (m_ipv4->GetInterfaceForDevice (idev) >= 0);
  uint32_t iif = m_ipv4->GetInterfaceForDevice (idev);
  cout << m_id <<" receive a packet\t"<<header.GetSource() << "\t"<<header.GetDestination()<<endl;

  if (header.GetDestination ().IsMulticast ())
    {
      NS_LOG_LOGIC ("Multicast destination-- returning false");
      return false; // Let other routing protocols try to handle this
    }

  if (header.GetDestination ().IsBroadcast ())
    {
      NS_LOG_LOGIC ("For me (Ipv4Addr broadcast address)");
      // TODO:  Local Deliver for broadcast
      // TODO:  Forward broadcast
    }

  if (ConfLoader::Instance()->getIndexSubnetMap()[m_id].contains(header.GetDestination())){
      NS_LOG_LOGIC ("For me (destination " << header.GetDestination() << " match)");
      cout << "Destination match" << endl;
      lcb (p, header, iif);
      return true;
  }

  // Check if input device supports IP forwarding
  if (m_ipv4->IsForwarding (iif) == false)
    {
      NS_LOG_LOGIC ("Forwarding disabled for this interface");
      ecb (p, header, Socket::ERROR_NOROUTETOHOST);
      return false;
    }
  // Next, try to find a route
  NS_LOG_LOGIC ("Unicast destination- looking up SRP Grid");
  Ptr<Ipv4Route> rtentry = LookupSRPGrid (header.GetDestination ());

  if (rtentry != 0)
    {
      NS_LOG_LOGIC ("Found unicast destination- calling unicast callback");

      ucb (rtentry, p, header);
      return true;
    }
  else
    {
      NS_LOG_LOGIC ("Did not find unicast destination- returning false");
      return false; // Let other routing protocols try to handle this
                    // route request.
    }
}
void 
Ipv4SRPRouting::NotifyInterfaceUp (uint32_t i)
{
  NS_LOG_FUNCTION (this << i);
  if (m_respondToInterfaceEvents && Simulator::Now ().GetSeconds () > 0)  // avoid startup events
    {
    }
}

void 
Ipv4SRPRouting::NotifyInterfaceDown (uint32_t i)
{
  NS_LOG_FUNCTION (this << i);
  if (m_respondToInterfaceEvents && Simulator::Now ().GetSeconds () > 0)  // avoid startup events
    {
    }
}

void 
Ipv4SRPRouting::NotifyAddAddress (uint32_t interface, Ipv4InterfaceAddress address)
{
  NS_LOG_FUNCTION (this << interface << address);
  if (m_respondToInterfaceEvents && Simulator::Now ().GetSeconds () > 0)  // avoid startup events
    {
    }
}

void 
Ipv4SRPRouting::NotifyRemoveAddress (uint32_t interface, Ipv4InterfaceAddress address)
{
  NS_LOG_FUNCTION (this << interface << address);
  if (m_respondToInterfaceEvents && Simulator::Now ().GetSeconds () > 0)  // avoid startup events
    {
    }
}

void 
Ipv4SRPRouting::SetIpv4 (Ptr<Ipv4> ipv4)
{
  NS_LOG_FUNCTION (this << ipv4);
  NS_ASSERT (m_ipv4 == 0 && ipv4 != 0);
  m_ipv4 = ipv4;
}


} // namespace ns3
