#include "srp-tag.h"

using namespace ns3; 

TypeId SRPTag::GetTypeId (void){
   static TypeId tid = TypeId ("ns3::SRPTag")
   		.SetParent<Tag> ()
   		.AddConstructor<SRPTag> ();
   	return tid;
}

TypeId SRPTag::GetInstanceTypeId (void) const{
	return GetTypeId ();
}

uint32_t SRPTag::GetSerializedSize (void) const{
	return 1;
}

void SRPTag::Serialize (TagBuffer i) const{
	return;
}

void SRPTag::Deserialize (TagBuffer i){
	return;
}

void SRPTag::Print (std::ostream &os) const{
	return;
}

void SRPTag::setUpList(vector<int> list){
	upList = list;
}
   
vector<int> SRPTag::getUpList(){
	return upList;
}
   
void SRPTag::add2UpList(int i){
	upList.push_back(i);
}

void SRPTag::setDownList(vector<int> list){
	downList = list;
}

vector<int> SRPTag::getDownList(){
	return downList;
}
   
void SRPTag::add2DownList(int i){
	downList.push_back(i);
}