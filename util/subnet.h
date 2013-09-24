#ifndef SUBNET_H
#define SUBNET_H

#include <stdint.h>
#include <string>
#include <iostream>

//#include "ns3/ipv4-address.h"

using namespace std;

namespace ns3{

class Subnet
{
public:
	Subnet();
	Subnet(uint32_t address, int mask);
	Subnet nextSubnet();
	Subnet prevSubnet();
	string toString();
	void setAddress(uint32_t address);
	void setMask(uint32_t mask);
	string uint32ToAddress(uint32_t x);
private:
	uint32_t address;
	uint32_t mask;
};

}

#endif /* SUBNET_H */