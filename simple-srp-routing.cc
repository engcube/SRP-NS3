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

#include "ns3/srp.h"
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

int 
main (int argc, char *argv[])
{
  // Users may find it convenient to turn on explicit debugging
  // for selected modules; the below lines suggest how to do this
//#if 0 
  LogComponentEnable ("SimpleSRPRoutingExample", LOG_LEVEL_INFO);
  //LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("SRPRoutingHelper", LOG_LEVEL_ALL);
  LogComponentEnable ("Ipv4SRPRouting", LOG_LEVEL_ALL);

//#endif

  //my code----------------------
  //-----------------------------
  //string filename = "/home/engcube/Workspace/epcc/dce/source/ns-3.17/src/srp/grid.ConfLoader::Instance()->;


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


  int total = ConfLoader::Instance()->getCoreNum()
            +ConfLoader::Instance()->getToRNum()
            +ConfLoader::Instance()->getBorderNum();
  //------------------------------
  //end of my code----------------


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
  
  //my code----------------------
  //-----------------------------
  NodeContainer c;
  c.Create (total);

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
      for(int j=CORE_NUM+TOR_NUM; j<total; j++){
          NodeContainer ninj = NodeContainer (c.Get(i), c.Get(j));
          nodeContainers.push_back(ninj);
      }
  }
  //------------------------------
  //end of my code----------------

  //NodeContainer c;
  //c.Create (4);
  //NodeContainer n0n2 = NodeContainer (c.Get (0), c.Get (2));
  //NodeContainer n1n2 = NodeContainer (c.Get (1), c.Get (2));
  //NodeContainer n3n2 = NodeContainer (c.Get (3), c.Get (2));
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
  
  //my code----------------------
  //-----------------------------
  list<NetDeviceContainer> netDeviceContainers;
  for(list<NodeContainer>::iterator it= nodeContainers.begin(); it!=nodeContainers.end(); ++it){
      NetDeviceContainer didj = p2p.Install (*it);
      netDeviceContainers.push_back(didj);
  }
  //------------------------------
  //end of my code----------------


  //NetDeviceContainer d0d2 = p2p.Install (n0n2);

  //NetDeviceContainer d1d2 = p2p.Install (n1n2);

  //p2p.SetDeviceAttribute ("DataRate", StringValue ("1500kbps"));
  //p2p.SetChannelAttribute ("Delay", StringValue ("10ms"));
  //NetDeviceContainer d3d2 = p2p.Install (n3n2);

  // Later, we add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  
  //my code----------------------
  //-----------------------------
  ipv4.SetBase("192.168.0.0","255.255.0.0");

  list<Ipv4InterfaceContainer> ipv4InterfaceContainers;
  for(list<NetDeviceContainer>::iterator it= netDeviceContainers.begin(); it!=netDeviceContainers.end(); ++it){
      Ipv4InterfaceContainer ii = ipv4.Assign (*it);
      ipv4InterfaceContainers.push_back(ii);
  }
  
  /*for(list<Ipv4InterfaceContainer>::iterator it= ipv4InterfaceContainers.begin(); it!=ipv4InterfaceContainers.end(); ++it){
      cout << it->GetAddress(0) << endl;
      cout << it->GetAddress(1) << endl;
  }*/

  //set up Grid-info
  /*for(int i=0; i<total; i++){
      ipv4SRPRoutingHelper.Create(c.Get(i));
  }*/

  /*map<int, Subnet> smap = ipv4SRPRoutingHelper.getIndexSubnetMap();
  for(map<int, Subnet>::iterator it = smap.begin(); it != smap.end(); ++it){
      cout << it->first <<"   "<<it->second.toString()<<endl;
  }
  for(int i=0; i<total; i++){
      //cout << c.Get(i)->GetObject<SRPRouter>()->GetSRPGrid() << endl;
      cout << c.Get(i)->GetObject<SRPRouter>()->GetSRPGrid()->toString() << endl;
  }*/

  //------------------------------
  //end of my code----------------


  //ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  //Ipv4InterfaceContainer i0i2 = ipv4.Assign (d0d2);

  //ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  //Ipv4InterfaceContainer i1i2 = ipv4.Assign (d1d2);

  //ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  //Ipv4InterfaceContainer i3i2 = ipv4.Assign (d3d2);

  // Create router nodes, initialize routing database and set up the routing
  // tables in the nodes.
  //ipv4SRPRoutingHelper.PopulateRoutingTables ();

  // Create the OnOff application to send UDP datagrams of size
  // 210 bytes at a rate of 448 Kb/s
  NS_LOG_INFO ("Create Applications.");
  
  uint16_t port = 9;   // Discard port (RFC 863)
  OnOffHelper onoff ("ns3::UdpSocketFactory", 
                     //Address (InetSocketAddress (ipv4InterfaceContainers.back().GetAddress (1), port)));
                     Address (InetSocketAddress ("10.0.1.2", port)));

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

  //OnOffHelper onoff ("ns3::UdpSocketFactory", 
  //                   Address (InetSocketAddress (i3i2.GetAddress (0), port)));
  //onoff.SetConstantRate (DataRate ("448kb/s"));
  //ApplicationContainer apps = onoff.Install (c.Get (0));
  //apps.Start (Seconds (1.0));
  //apps.Stop (Seconds (10.0));

  // Create a packet sink to receive these packets
  //PacketSinkHelper sink ("ns3::UdpSocketFactory",
  //                       Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  //apps = sink.Install (c.Get (3));
  //apps.Start (Seconds (1.0));
  //apps.Stop (Seconds (10.0));

  // Create a similar flow from n3 to n1, starting at time 1.1 seconds
  //onoff.SetAttribute ("Remote", 
  //                    AddressValue (InetSocketAddress (i1i2.GetAddress (0), port)));
  //apps = onoff.Install (c.Get (3));
  //apps.Start (Seconds (1.1));
  //apps.Stop (Seconds (10.0));

  // Create a packet sink to receive these packets
  //apps = sink.Install (c.Get (1));
  //apps.Start (Seconds (1.1));
  //apps.Stop (Seconds (10.0));

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

  for(int i=0; i<total; i++){
      cout << i << "  " << c.Get(i)->GetObject<SRPRouter>()->GetRoutingProtocol()->GetSRPGrid()->toString() << endl;
  }
  /*for(int i=0; i<total; i++){
      cout << i << endl;
     Ptr<Ipv4> m_ipv4 = c.Get(i)->GetObject<SRPRouter>()->GetRoutingProtocol()->getIpv4();
      for (uint32_t m = 0; m < m_ipv4->GetNInterfaces (); m++)
      {
      for (uint32_t n = 0; n < m_ipv4->GetNAddresses (m); n++)
        {
          Ipv4InterfaceAddress iaddr = m_ipv4->GetAddress (m, n);
          cout << m<<" "<<n<<" "<<iaddr << endl;
        }
      }
      cout << endl;
  }*/
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (11));
  Simulator::Run ();
  NS_LOG_INFO ("Done.");

  if (enableFlowMonitor)
    {
     flowmon->SerializeToXmlFile ("simple-SRP-routing.flowmon", false, false);
    }

  Simulator::Destroy ();



  return 0;
}
