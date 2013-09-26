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
	Subnet nextSubnet() const;
	Subnet prevSubnet() const;
	string toString() const;
	void setAddress(uint32_t address);
	void setMask(uint32_t mask);
	uint32_t getAddress() const;
	uint32_t getMask() const;
	string uint32ToAddress(uint32_t x) const;
	bool equals(Subnet other) const;
private:
	uint32_t address;
	uint32_t mask;
};

}

#endif /* SUBNET_H */