#ifndef CONF_LOADER_H
#define CONF_LOADER_H

#include <string>
#include <iostream>

using namespace std;

namespace ns3{

class ConfLoader
{
public:
	ConfLoader(string filename);
private:
	void read(string filename);
	int coreNum;
	int torNum;
	int borderNum;
};

}

#endif /* CONF_LOADER_H */