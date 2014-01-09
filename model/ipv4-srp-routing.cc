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
#include "ns3/srp-tag.h"
#include "ns3/core-module.h"
#include "ns3/queue.h"

#include <sstream>
#include <algorithm>

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

//Ipv4SRPRouting::Ipv4SRPRouting (){
//}

Ipv4SRPRouting::Ipv4SRPRouting () 
  : m_randomEcmpRouting (false),
    m_respondToInterfaceEvents (false)
{
  NS_LOG_FUNCTION (this);
  m_update_state = false;
  m_rand = CreateObject<UniformRandomVariable> ();
  //m_SRPRoutingTable = map<Subnet, int>();
}

Ipv4SRPRouting::~Ipv4SRPRouting ()
{
  NS_LOG_FUNCTION (this);
}

void Ipv4SRPRouting::SetSRPRoutingTable(map<Subnet, vector<int> >& grid){
    this->m_SRPRoutingTable = grid;
}

map<Subnet, vector<int> >& Ipv4SRPRouting::GetSRPRoutingTable (void){
    return this->m_SRPRoutingTable;
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

bool Ipv4SRPRouting::update(){
  //cout << "<<<<in update  " << m_id << endl;
  NS_LOG_DEBUG(Simulator::Now() << "update  " << m_id );  
  
  CheckTxQueue();
  return true;
}

void Ipv4SRPRouting::addToNeighbors(int neighbor, Time time){
    m_CurNeighbors[neighbor] = time;
}
  
map<int, Time>& Ipv4SRPRouting::getNeighbors(){
    return m_CurNeighbors;
}

void Ipv4SRPRouting::removeFromNeighbors(int neighbor){
    m_CurNeighbors.erase(neighbor);
}

void Ipv4SRPRouting::addToLinkStateDatabase(int node, int cost){
    m_LinkStateDatabase[node] = cost;
}

map<int, int>& Ipv4SRPRouting::getLinkStateDatabase(){
    return m_LinkStateDatabase;
}
  
void Ipv4SRPRouting::removeFromLinkStateDatabase(int node){
    m_LinkStateDatabase.erase(node);
}

void Ipv4SRPRouting::initGrid(){
    m_Grid.clear();        
    int add = ConfLoader::Instance()->getBorderNum()+ConfLoader::Instance()->getToRNum();

    if(m_id < ConfLoader::Instance()->getCoreNum()){
        for(int i= ConfLoader::Instance()->getCoreNum(); i<ConfLoader::Instance()->getCoreNum()+ConfLoader::Instance()->getToRNum(); i++){
            map<int, int> grid_line;
            grid_line[i] = 1;
            m_Grid[add+i] = grid_line;
        }
    }else if(m_id<ConfLoader::Instance()->getTotalNum()){
        for(int i= ConfLoader::Instance()->getCoreNum(); i<ConfLoader::Instance()->getCoreNum()+ConfLoader::Instance()->getToRNum(); i++){
            if(i==m_id){
                continue;
            }
            map<int, int> grid_line;
              for(int j = 0; j<ConfLoader::Instance()->getCoreNum(); j++){
                  grid_line[j] = 1;
              }
            m_Grid[add+i] = grid_line;  
        }        
    }
    //cout << toGridString() << endl;

    reCalculate();
}

string Ipv4SRPRouting::toGridString(){
    stringstream result;
    result << m_id << "\tGrid:" << endl;
    for(map<int, map<int,int> >::iterator it = m_Grid.begin(); it != m_Grid.end(); ++it){
        result << it->first << ":\t\t";
        for(map<int,int>::iterator it2 = it->second.begin(); it2!=it->second.end(); ++it2){
            result << it2->first << ":" << it2->second << "\t";
        }
        result << endl;
    }
    return result.str();
}

string Ipv4SRPRouting::toString(){
    stringstream result;
    result << m_id << "\tRoutingTable:" << endl;
    for(map<Subnet, vector<int> >::iterator it = m_SRPRoutingTable.begin(); it != m_SRPRoutingTable.end(); ++it){
        std::vector<int> v = it->second;
        for(std::vector<int>::iterator it2 = v.begin(); it2!=v.end(); ++it2){
            result << it->first.toString() << "\t" << *it2 << "\n";
        }
    }
    return result.str();
}

void Ipv4SRPRouting::sendHelloMessage(){
    Ptr<Packet> packet = Create<Packet>(1);
    SRPTag tag;
    tag.setType(1);
    tag.setNode(m_id);
    packet->AddPacketTag(tag);

    Ptr<Socket> m_socket = Socket::CreateSocket (ConfLoader::Instance()->getNodeContainer().Get(m_id), TypeId::LookupByName ("ns3::UdpSocketFactory"));
    m_socket->SetAllowBroadcast(true);
    m_socket->Bind ();
    m_socket->Connect (Address (InetSocketAddress ("255.255.255.255", 9)));
    m_socket->Send (packet);
    //cout << m_id << " send a hello message" << endl;
}

int Ipv4SRPRouting::diffVector(vector<int>& list1, vector<int>& list2){
    std::vector<int> upList;
    std::vector<int> downList;
    for(std::vector<int>::iterator it = list1.begin(); it!=list1.end(); ++it){
        if(find(list2.begin(), list2.end(), *it)==list2.end()){
            downList.push_back(*it);
        }
    }
    for(std::vector<int>::iterator it = list2.begin(); it!=list2.end(); ++it){
        if(find(list1.begin(), list1.end(), *it)==list1.end()){
            upList.push_back(*it);
        }
    }
    if(upList.size()==0&&downList.size()==0){ return -1;}
    int index = ConfLoader::Instance()->getLSANum();
    ConfLoader::Instance()->addLSA(index, upList, downList);
    cout << m_id<<"Updating Grid" << endl;
    return index;
};

void Ipv4SRPRouting::DoCalculate(){
    map<Subnet, vector<int> > routingTable;
    for(map<int, map<int, int> >::iterator it = m_Grid.begin(); it!=m_Grid.end(); ++it){
        map<int, int> grid_line = it->second;
        Subnet node = ConfLoader::Instance()->getSubnetByNode(it->first);
        std::vector<int> v;
        for(map<int, int>::iterator it2 = grid_line.begin(); it2!=grid_line.end(); ++it2){
            if(it2->second%2==1){
                v.push_back(ConfLoader::Instance()->calcSourceInterfaceByNode(m_id, it2->first));
            }
        }
        routingTable[node] = v;
    }
    if(m_id >=ConfLoader::Instance()->getCoreNum()&& m_id < ConfLoader::Instance()->getCoreNum()+ConfLoader::Instance()->getToRNum()){
        std::vector<int> v;
        v.push_back(ConfLoader::Instance()->getCoreNum()+1);
        routingTable[ConfLoader::Instance()->getSubnetByNode(m_id+ConfLoader::Instance()->getBorderNum()+ConfLoader::Instance()->getToRNum())] = v;
    }
    if(m_id >= ConfLoader::Instance()->getTotalNum()){
        for(int i = ConfLoader::Instance()->getTotalNum(); i < ConfLoader::Instance()->getTotalNum()+ConfLoader::Instance()->getToRNum(); i++){
            std::vector<int> v;
            if(i==m_id){
              v.push_back(0);
            }else{
              v.push_back(1);
            }
            routingTable[ConfLoader::Instance()->getSubnetByNode(i)] = v;
        }
    }
    m_SRPRoutingTable = routingTable;
    cout << toString() << endl;  
}

void Ipv4SRPRouting::reCalculate(){
    Simulator::Schedule (Seconds (ConfLoader::Instance()->getCalculateCost()), &Ipv4SRPRouting::DoCalculate, this);
}

void Ipv4SRPRouting::checkDiff(int node, int index){

    vector<int> upLSA = ConfLoader::Instance()->getUpLSA(index);
    vector<int> downLSA = ConfLoader::Instance()->getDownLSA(index);
    vector<int> lastAvailableNodes = getAvailableNodes(m_Grid);

    for(std::vector<int>::iterator it = upLSA.begin(); it!=upLSA.end(); ++it){
        if(m_Grid.find(*it)!=m_Grid.end() && m_Grid[*it].find(node)!= m_Grid[*it].end()){
            if(m_Grid[*it][node]%2==0){
                m_Grid[*it][node] ++;
            }
        }
    }
    for(std::vector<int>::iterator it = downLSA.begin(); it!=downLSA.end(); ++it){
        if(m_Grid.find(*it)!=m_Grid.end() && m_Grid[*it].find(node)!= m_Grid[*it].end()){
            if(m_Grid[*it][node]%2==1){
                m_Grid[*it][node] --;
            }
        }
    }
    //check if m_Grid has update
    vector<int> curAvailableNodes = getAvailableNodes(m_Grid);

    reCalculate();

    int num = diffVector(lastAvailableNodes, curAvailableNodes);
    if( num<0 ){
        cout << m_id <<" no change" <<endl;
        return;
    }
    sendLSAMessage(num);
}

void Ipv4SRPRouting::sendLSAMessage(int num){
    //m_LSAs[node] = ConfLoader::Instance()->getLSA(index);
    Ptr<Packet> packet = Create<Packet>(1);
    SRPTag tag;
    tag.setType(2);
    tag.setNode((uint16_t)m_id);
    tag.setLSA((uint32_t)num);
    //tag.setLSAIndex();
    packet->AddPacketTag(tag);

    Ptr<Socket> m_socket = Socket::CreateSocket (ConfLoader::Instance()->getNodeContainer().Get(m_id), TypeId::LookupByName ("ns3::UdpSocketFactory"));
    m_socket->SetAllowBroadcast(true);
    m_socket->Bind ();
    m_socket->Connect (Address (InetSocketAddress ("255.255.255.255", 9)));
    m_socket->Send (packet);
}

vector<int> Ipv4SRPRouting::getAvailableNodes(map<int, map<int, int> >& grid){
    std::vector<int> v;
    for(map<int, map<int, int> >::iterator it = grid.begin(); it!=grid.end(); ++it){
        map<int, int> item = it->second;
        for(map<int, int>::iterator it2 = item.begin(); it2 != item.end(); ++it2){
            if(it2->second%2==1){
                v.push_back(it->first);
                break;
            }
        }
    }
    return v;
}

void Ipv4SRPRouting::toString(vector<uint16_t>& v){
    for(int i=0; i< (int)v.size(); i++){
      cout << (int)v[i] << " ";
    }
    cout << endl;
}

void Ipv4SRPRouting::handleMessage(Ptr<const Packet> packet){
          SRPTag tag;
          uint8_t type;
          uint16_t from;
          bool found = packet->PeekPacketTag(tag);
          if (found){
            type = tag.getType();
            from = tag.getNode();
            //cout << from;
            if(type == 1){
                //cout << "receive hello message" << endl;
                addToNeighbors(from, Simulator::Now());
            }else if(type == 2){
                //cout << "receive update message from " << (int)from << endl;
                uint32_t index = tag.getLSAIndex();
                checkDiff(from, index);
            }else{
                //cout << "receive not-hello message" << endl;
            }
          }else{
              //cout << "receive normal message" << endl;
              ConfLoader::Instance()->prepareLinkDown();
          }

}

void Ipv4SRPRouting::addNeighbor(int node){
    m_DownLink.erase(std::remove(m_DownLink.begin(), m_DownLink.end(), node), m_DownLink.end());
    vector<int> lastAvailableNodes = getAvailableNodes(m_Grid);
    for(map<int, map<int, int> >::iterator it = m_Grid.begin(); it!=m_Grid.end(); ++it){
        map<int, int> item = it->second;
        for(map<int, int>::iterator it2 = item.begin(); it2 != item.end(); ++it2){
            if(node==it2->first && (it2->second%2==0)){
              it2->second++;
              break;
            }
        }
        it->second = item;
    }
    vector<int> curAvailableNodes = getAvailableNodes(m_Grid);
    reCalculate();

    int num = diffVector(lastAvailableNodes, curAvailableNodes);
    if( num<0 ){
        return;
    }
    sendLSAMessage(num);
}

void Ipv4SRPRouting::removeNeighbor(int node){
    m_DownLink.push_back(node);
    vector<int> lastAvailableNodes = getAvailableNodes(m_Grid);
    for(map<int, map<int, int> >::iterator it = m_Grid.begin(); it!=m_Grid.end(); ++it){
        map<int, int> item = it->second;
        for(map<int, int>::iterator it2 = item.begin(); it2 != item.end(); ++it2){
            if(node==it2->first && (it2->second%2==1)){
              it2->second--;
              break;
            }
        }
        it->second = item;
    }
    vector<int> curAvailableNodes = getAvailableNodes(m_Grid);
    reCalculate();

    int num = diffVector(lastAvailableNodes, curAvailableNodes);
    if( num<0 ){
        return;
    }
    sendLSAMessage(num);
}

void Ipv4SRPRouting::updateNeighbors(){
    vector<uint16_t> lsa;
    for(map<int, Time>::iterator it = m_CurNeighbors.begin(); it != m_CurNeighbors.end(); ++it){
            lsa.push_back((uint16_t)it->first);
    }
    //cout << "neighbors change" << endl;
    //int index = ConfLoader::Instance()->getLSANum();
    //ConfLoader::Instance()->addLSA(index, lsa);
    //sendLSAMessage(m_id, index);
    //m_LastNeighbors = m_CurNeighbors;
}

void Ipv4SRPRouting::checkNeighbors(){
    //cout << m_id << " now has " << m_LSAs.size() << endl;

    /*Time now = Simulator::Now();
    for(map<int, Time>::iterator it = m_CurNeighbors.begin(); it != m_CurNeighbors.end(); ++it){
        if(now - it->second > Seconds(ConfLoader::Instance()->getUnavailableInterval())){
            m_CurNeighbors.erase(it);
        }
    }
    bool toNotify = false;
    for(map<int, Time>::iterator it = m_CurNeighbors.begin(); it != m_CurNeighbors.end(); ++it){
        if(m_LastNeighbors.find(it->first) == m_LastNeighbors.end()){
            toNotify = true;
        }
    }

    for(map<int, Time>::iterator it = m_LastNeighbors.begin(); it != m_LastNeighbors.end(); ++it){
        if(m_CurNeighbors.find(it->first) == m_CurNeighbors.end()){
            toNotify = true;
        }
    }
    if(toNotify){
        vector<uint16_t> lsa;
        for(map<int, Time>::iterator it = m_CurNeighbors.begin(); it != m_CurNeighbors.end(); ++it){
            lsa.push_back((uint16_t)it->first);
        }
        //cout << "neighbors change" << endl;
        int index = ConfLoader::Instance()->getLSANum();
        ConfLoader::Instance()->addLSA(index, lsa);
        sendLSAMessage(m_id, index);
    }
    m_LastNeighbors = m_CurNeighbors;*/
}

void Ipv4SRPRouting::send2Peer(Ptr<Packet> packet){
}

void Ipv4SRPRouting::sendMessage(Ipv4Address ip, Ptr<Packet> packet){
  /*Ptr<Socket> m_socket = Socket::CreateSocket (ConfLoader::Instance()->getNodeContainer().Get(m_id), TypeId::LookupByName ("ns3::UdpSocketFactory"));
  m_socket->Bind ();
  m_socket->Connect (Address (InetSocketAddress (ip, 9)));
  m_socket->Send (packet);*/
}


Ptr<Ipv4Route> Ipv4SRPRouting::LookupSRPRoutingTable (Ipv4Address source, Ipv4Address dest)
{
  NS_LOG_LOGIC ("Looking for route for destination " << dest);
  int out_interface = -1;
  for(map<Subnet, vector<int> >::iterator it = m_SRPRoutingTable.begin(); it != m_SRPRoutingTable.end(); ++it){
      if(it->first.contains(dest)){
          //out_interface = it->second;
          int size = it->second.size();
          if(size==0) break;
          //ECMP hash
          int choice = (int)(source.Get()+dest.Get()) % size;
          out_interface = it->second[choice];
          break;
      }
  }
  if(out_interface == -1){
      cout << "No route found!" << endl;
      return 0;
  }
  int destNode = ConfLoader::Instance()->calcDestNodeBySource(m_id, out_interface);
  int destInterface = ConfLoader::Instance()->calcDestInterfaceBySource(m_id, out_interface);
  Ptr<Ipv4> to_ipv4 = ConfLoader::Instance()->getNodeContainer().Get(destNode)->GetObject<Ipv4SRPRouting>()->getIpv4();
  cout << Simulator::Now() << "Route from this node "<<m_id <<" on interface " << out_interface <<" to Node " << destNode << " on interface " << destInterface << endl;

  Ptr<Ipv4Route> rtentry = Create<Ipv4Route> ();
  rtentry->SetDestination (to_ipv4->GetAddress (destInterface, 0).GetLocal ());
  rtentry->SetSource (m_ipv4->GetAddress (out_interface, 0).GetLocal ());
  rtentry->SetGateway (Ipv4Address("0.0.0.0"));
  rtentry->SetOutputDevice (m_ipv4->GetNetDevice (out_interface));
  return rtentry;
}


void Ipv4SRPRouting::CheckTxQueue(){

    m_LastCheckTime = Simulator::Now();
    vector<int> lastAvailableNodes = getAvailableNodes(m_Grid);
    
    for(map<int, map<int, int> >::iterator it = m_Grid.begin(); it!=m_Grid.end(); ++it){
        map<int, int> item = it->second;
        for(map<int, int>::iterator it2 = item.begin(); it2 != item.end(); ++it2){
            if(it2->second%2==1){
                it2->second--;
            }
        }
        it->second = item;
    }

    int n = m_ipv4->GetNInterfaces();
    for(int i=1; i< n; i++){
      PointerValue ptr;
      m_ipv4->GetNetDevice (i)->GetAttribute("TxQueue", ptr);
      int current = ptr.Get<Queue> ()->GetNPackets() ;

      UintegerValue limit;
      ptr.Get<Queue> ()->GetAttribute ("MaxPackets", limit);
      int total = limit.Get ();

      float percent = current*1.0/total;
      cout << i << "/" << n << " ;Percent: " << percent <<" ;Total: " << total << " ;Current: " << current << endl;

      if(percent<=ConfLoader::Instance()->getCongestionWaningLimit()){
          int node = ConfLoader::Instance()->getNodeByInterface(m_id,i);
          if(find(m_DownLink.begin(), m_DownLink.end(), node)==m_DownLink.end()){
                  for(map<int, map<int, int> >::iterator it = m_Grid.begin(); it!=m_Grid.end(); ++it){
                      map<int, int> item = it->second;
                      for(map<int, int>::iterator it2 = item.begin(); it2 != item.end(); ++it2){
                          if(node==it2->first && (it2->second%2==0)){
                            it2->second++;
                            break;
                          }
                      }
                      it->second = item;
                  }
          }
      }else{
          cout << m_id << "interface " << i << "exceed" << endl;
      }
    }
    
    vector<int> curAvailableNodes = getAvailableNodes(m_Grid);
    reCalculate();

    int num = diffVector(lastAvailableNodes, curAvailableNodes);
    if( num<0 ){
        return;
    }
    sendLSAMessage(num);
}

Ptr<Ipv4Route>
Ipv4SRPRouting::RouteOutput (Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr)
{
  ConfLoader::Instance()->incrementSendPacket(m_id);
  NS_LOG_FUNCTION (this << p << &header << oif << &sockerr);
  NS_LOG_DEBUG( Simulator::Now() << " " << m_id <<" send a packet\t"<< p << "\t" << header.GetSource() << "\t"<<header.GetDestination());
  cout << Simulator::Now() << " " << m_id <<" send a packet\t"<< p << "\t" << header.GetSource() << "\t"<<header.GetDestination() << endl;
  NS_LOG_LOGIC ("Unicast destination- looking up");
  Ptr<Ipv4Route> rtentry = LookupSRPRoutingTable (header.GetSource(), header.GetDestination ());
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
  NS_LOG_DEBUG( Simulator::Now() << " " << m_id <<" receive a packet\t"<< p << "\t" << header.GetSource() << "\t"<<header.GetDestination() );
  cout << Simulator::Now() << " " << m_id <<" receive a packet\t"<< p << "\t" << header.GetSource() << "\t"<<header.GetDestination() << endl;
  for (uint32_t j = 0; j < m_ipv4->GetNInterfaces (); j++)
    {
      for (uint32_t i = 0; i < m_ipv4->GetNAddresses (j); i++)
        {
          Ipv4InterfaceAddress iaddr = m_ipv4->GetAddress (j, i);
          Ipv4Address addr = iaddr.GetLocal ();
          if (addr.IsEqual (header.GetDestination ()))
            {
              if (j == iif)
                {
                  NS_LOG_LOGIC ("For me (destination " << addr << " match)");
                }
              else
                {
                  NS_LOG_LOGIC ("For me (destination " << addr << " match) on another interface " << header.GetDestination ());
                }
              lcb (p, header, iif);
              //cout << "destination match!" <<endl;
              return true;
            }
          if (header.GetDestination ().IsEqual (iaddr.GetBroadcast ()))
            {
              NS_LOG_LOGIC ("For me (interface broadcast address)");
              lcb (p, header, iif);
              return true;
            }
          NS_LOG_LOGIC ("Address "<< addr << " not a match");
        }
    }
    if (m_ipv4->IsForwarding (iif) == false)
    {
      NS_LOG_LOGIC ("Forwarding disabled for this interface");
      ecb (p, header, Socket::ERROR_NOROUTETOHOST);
      return false;
    }
  // Next, try to find a route
  NS_LOG_LOGIC ("Unicast destination- looking up global route");
  Ptr<Ipv4Route> rtentry = LookupSRPRoutingTable (header.GetSource(), header.GetDestination ());
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
