/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

//
// Network topology
//
//  n0
//     \ 5 Mb/s, 2ms
//      \          1.5Mb/s, 10ms
//       n2 -------------------------n3
//      /
//     / 5 Mb/s, 2ms
//   n1
//
// - all links are point-to-point links with indicated one-way BW/delay
// - CBR/UDP flows from n0 to n3, and from n3 to n1
// - FTP/TCP flow from n0 to n3, starting at time 1.2 to time 1.35 sec.
// - UDP packet size of 210 bytes, with per-packet interval 0.00375 sec.
//   (i.e., DataRate of 448,000 bps)
// - DropTail queues 
// - Tracing of queues and packet receptions to file "simple-srp-routing.tr"

#include <iostream>
#include <fstream>
#include <string>
#include <list>

#include <cassert>

//#include "ns3/srp.h"
#include "ns3/conf-loader.h"
#include "ns3/subnet.h"
#include "ns3/srp-router-interface.h"
#include "ns3/ipv4-srp-routing-helper.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("SimpleSRPRoutingExample");

void action(){
    //ConfLoader::Instance()->setNodeState(0,false);
    //ConfLoader::Instance()->setNodeState(3,false);
}

void update(){
  action();
  for(int i=0; i<ConfLoader::Instance()->getTotalNum();i++){
    if(ConfLoader::Instance()->getNodeContainer().Get(i)->GetObject<SRPRouter>()->update()){
      // if one SRP router found that Grid have changed, then there is no need to update others
      break;
    }
  }
  //ConfLoader::Instance()->clearNodeActions();
  //ConfLoader::Instance()->clearLinkActions();
}

int 
main (int argc, char *argv[])
{
  // Users may find it convenient to turn on explicit debugging
  // for selected modules; the below lines suggest how to do this
#if 1
  LogComponentEnable ("SimpleSRPRoutingExample", LOG_LEVEL_INFO);
  //LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);
  //LogComponentEnable ("SRPRoutingHelper", LOG_LEVEL_ALL);
  //LogComponentEnable ("Ipv4SRPRouting", LOG_LEVEL_ALL);
  //LogComponentEnable ("Ipv4L3Protocol", LOG_LEVEL_ALL);

#endif
  int CORE_NUM = 2;
  int TOR_NUM = 4;
  int BORDER_NUM = 2;
  int SUBNET_MASK = 24;
  uint32_t ADDRESS_START = 0x0a000000; // 10.0.0.1

  ConfLoader::Instance()->setCoreNum(CORE_NUM);
  ConfLoader::Instance()->setToRNum(TOR_NUM);
  ConfLoader::Instance()->setBorderNum(BORDER_NUM);
  ConfLoader::Instance()->setSubnetMask(SUBNET_MASK);
  ConfLoader::Instance()->setAddressStart(ADDRESS_START);

  cout << "Core number:" << ConfLoader::Instance()->getCoreNum() << endl;
  cout << "ToR number:" << ConfLoader::Instance()->getToRNum() << endl;
  cout << "Border number:" << ConfLoader::Instance()->getBorderNum() << endl;
  cout << "Netmask number:" << ConfLoader::Instance()->getSubnetMask() << endl;

  // Set up some default values for the simulation.  Use the 
  Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (210));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("448kb/s"));

  //DefaultValue::Bind ("DropTailQueue::m_maxPackets", 30);

  // Allow the user to override any of the defaults and the above
  // DefaultValue::Bind ()s at run-time, via command-line arguments
  CommandLine cmd;
  bool enableFlowMonitor = false;
  cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
  cmd.Parse (argc, argv);

  // Here, we will explicitly create four nodes.  In more sophisticated
  // topologies, we could Configure a node factory.
  NS_LOG_INFO ("Create nodes.");
  

  NodeContainer c;
  c.Create (ConfLoader::Instance()->getTotalNum());

  ConfLoader::Instance()->setNodeContainer(c);

  Subnet subnet(ADDRESS_START, SUBNET_MASK);
  ConfLoader::Instance()->addItem2IndexSubnetMap(CORE_NUM, subnet);
  for(int i=CORE_NUM+1; i<CORE_NUM+TOR_NUM; i++){
      Subnet s = ConfLoader::Instance()->getIndexSubnetMap()[i-1].nextSubnet();
      ConfLoader::Instance()->addItem2IndexSubnetMap(i, s);
  }

  list<NodeContainer> nodeContainers;

  for(int i=0; i<CORE_NUM; i++){
      for(int j=CORE_NUM; j<CORE_NUM+TOR_NUM; j++){
          NodeContainer ninj = NodeContainer (c.Get(i), c.Get(j));
          nodeContainers.push_back(ninj);
      }
  }

  for(int i=0; i<CORE_NUM; i++){
      for(int j=CORE_NUM+TOR_NUM; j<ConfLoader::Instance()->getTotalNum(); j++){
          NodeContainer ninj = NodeContainer (c.Get(i), c.Get(j));
          nodeContainers.push_back(ninj);
      }
  }

  // inital node states
  map<int, bool> nodeStates;
  for(int i=0; i< ConfLoader::Instance()->getTotalNum(); i++){
      nodeStates.insert(make_pair(i, true));
  }
  ConfLoader::Instance()->setNodeStates(nodeStates);
  //ConfLoader::Instance()->setNodeState(0,false);
  // inital link states
  map<pair<int,int>, bool> linkStates;
  for(int j=0;j<ConfLoader::Instance()->getCoreNum();j++){
    for(int i=ConfLoader::Instance()->getCoreNum(); i< ConfLoader::Instance()->getTotalNum(); i++){
      linkStates.insert(make_pair(make_pair(j,i), true));
    }
  }
  for(int j=ConfLoader::Instance()->getCoreNum()+ConfLoader::Instance()->getToRNum();j<ConfLoader::Instance()->getTotalNum();j++){
    for(int i=j+1; i< ConfLoader::Instance()->getTotalNum(); i++){
      linkStates.insert(make_pair(make_pair(j,i), true));
    }
  }
  ConfLoader::Instance()->setLinkStates(linkStates);
  //ConfLoader::Instance()->setLinkState(6,7,false);

  InternetStackHelper internet;

  Ipv4ListRoutingHelper listRouting;
  Ipv4StaticRoutingHelper staticRouting;
  Ipv4SRPRoutingHelper ipv4SRPRoutingHelper;
  listRouting.Add (staticRouting, 0);
  listRouting.Add (ipv4SRPRoutingHelper, 10);

  internet.SetRoutingHelper (listRouting);
  internet.Install (c);

  // We create the channels first without any IP addressing information
  NS_LOG_INFO ("Create channels.");
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("0.5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("200ms"));
  
  list<NetDeviceContainer> netDeviceContainers;
  for(list<NodeContainer>::iterator it= nodeContainers.begin(); it!=nodeContainers.end(); ++it){
      NetDeviceContainer didj = p2p.Install (*it);
      netDeviceContainers.push_back(didj);
  }

  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  
  ipv4.SetBase("192.168.0.0","255.255.0.0");

  list<Ipv4InterfaceContainer> ipv4InterfaceContainers;
  for(list<NetDeviceContainer>::iterator it= netDeviceContainers.begin(); it!=netDeviceContainers.end(); ++it){
      Ipv4InterfaceContainer ii = ipv4.Assign (*it);
      ipv4InterfaceContainers.push_back(ii);
  }
  
  for(int i=0; i<ConfLoader::Instance()->getTotalNum();i++){
      Ptr<Ipv4> m_ipv4 = ConfLoader::Instance()->getNodeContainer().Get(i)->GetObject<SRPRouter>()
                          ->GetRoutingProtocol()->getIpv4();
      for (uint32_t j = 1; j < m_ipv4->GetNInterfaces (); j++){
          Ipv4Address addr = m_ipv4->GetAddress (j, 0).GetLocal();
          ConfLoader::Instance()->addItem2Ipv4IndexMap(addr,i);
      }
  }
  /*map<Ipv4Address, int> tmpMap = ConfLoader::Instance()->getIpv4IndexMap();
  cout << tmpMap.size() << endl;
  for(map<Ipv4Address, int>::iterator it = tmpMap.begin() ; it!=tmpMap.end() ; ++it){
            cout << it->first << " " << it->second << endl;
  }
//bug here!!!
  for(map<Ipv4Address, int>::iterator it=ConfLoader::Instance()->getIpv4IndexMap().begin();it!=ConfLoader::Instance()->getIpv4IndexMap().end();++it){
	cout << it->first << " " << it->second << endl;
	}
*/
	NS_LOG_INFO ("Create Applications.");

  uint16_t port = 9;   // Discard port (RFC 863)
  OnOffHelper onoff ("ns3::UdpSocketFactory", 
                     //Address (InetSocketAddress (ipv4InterfaceContainers.back().GetAddress (1), port)));
                    //Address (InetSocketAddress ("10.0.3.2", port)));
                    Address (InetSocketAddress ("192.168.0.16", port)));

  onoff.SetConstantRate (DataRate ("448kb/s"));
  //source: the first ToR node
  ApplicationContainer apps = onoff.Install (c.Get (CORE_NUM));

  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (1.01));
  
  // Create a packet sink to receive these packets
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  for(int i=CORE_NUM; i< CORE_NUM+TOR_NUM;i++){  
    apps = sink.Install (c.Get (i));
  }
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (1.01));

  AsciiTraceHelper ascii;
  p2p.EnableAsciiAll (ascii.CreateFileStream ("simple-SRP-routing.tr"));
  p2p.EnablePcapAll ("simple-SRP-routing");

  // Flow Monitor
  Ptr<FlowMonitor> flowmon;
  FlowMonitorHelper flowmonHelper;
  if (enableFlowMonitor)
    {
      flowmon = flowmonHelper.InstallAll ();
    }
  
  for(int i=0; i<ConfLoader::Instance()->getTotalNum(); i++){
      cout << i << "  " << c.Get(i)->GetObject<SRPRouter>()->GetRoutingProtocol()->GetSRPGrid()->toString() << endl;
  }

  int simulateTime = 11;
  int simulateInterval = 3;
  for(int i=1; i<simulateTime/simulateInterval;i++){
    Time onInterval = Seconds (i*simulateInterval);
    Simulator::Schedule (onInterval, &update);
  }

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (simulateTime));
  Simulator::Run ();

  NS_LOG_INFO ("Done.");

  if (enableFlowMonitor)
    {
     flowmon->SerializeToXmlFile ("simple-SRP-routing.flowmon", false, false);
    }

  Simulator::Destroy ();
  


  return 0;
}
