#include <fstream>
#include <iostream>

#include "conf-loader.h"
//#include "ns3/log.h"


//NS_LOG_COMPONENT_DEFINE ("ConfLoader");

namespace ns3{

using namespace std;

ConfLoader::ConfLoader(string filename){
	coreNum = 4;
	torNum = 128;
	borderNum = 2;
	read(filename);
}

void ConfLoader::read(string filename){
	ifstream fin(filename.c_str(), ifstream::in);
	string s;
	while(getline(fin,s)){
		cout << s << endl;
	}
}

}