
#include "subnet.h"

#include <cmath>
#include <sstream>
#include <bitset>

namespace ns3{

Subnet::Subnet(){
}

Subnet::Subnet(uint32_t address, int mask){
	uint32_t mask_tmp = 0;
	if (mask>0 && mask < 32){
		for(int i=31;i>=32-mask;i--){
			mask_tmp += (uint32_t)pow(2, i);
			//cout << mask_tmp << endl;
		}
	}else{
		cout << "invaild parameters" << endl;
		return;
	}
	if((address&(~mask_tmp))!=0){
		cout << "invaild parameters" << endl;
		return;
	}
	this->address = address;
	this->mask = mask_tmp;
}

void Subnet::setAddress(uint32_t address){
	this->address = address;
}

void Subnet::setMask(uint32_t mask){
	this->mask = mask;
}

Subnet Subnet::nextSubnet(){
	Subnet subnet;
	subnet.setAddress(this->address + (~this->mask+1));
	subnet.setMask(this->mask);
	return subnet;
}

Subnet Subnet::prevSubnet(){
	Subnet subnet;
	subnet.setAddress(this->address - (~this->mask+1));
	subnet.setMask(this->mask);
	return subnet;
}

string Subnet::uint32ToAddress(uint32_t x){
	int a = x>>24;
	int b = (x<<8)>>24;
	int c = (x<<16)>>24;
	int d = (x<<24)>>24;
	stringstream ss;
	ss << a << "." << b << "." << c << "." << d;
	return ss.str();
}

string Subnet::toString(){
	return uint32ToAddress(this->address)+"/"+uint32ToAddress(this->mask);
}

uint32_t Subnet::getAddress(){
	return this->address;
}

uint32_t Subnet::getMask(){
	return this->mask;
}

bool Subnet::equals(Subnet other){
	if(other.getMask()==this->mask && other.getAddress()==this->address){
		return TRUE;
	}
	return FALSE;
}

}