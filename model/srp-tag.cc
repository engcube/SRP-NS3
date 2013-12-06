#include "srp-tag.h"
#include "ns3/log.h"

using namespace ns3; 

NS_LOG_COMPONENT_DEFINE ("SRPTag");

NS_OBJECT_ENSURE_REGISTERED (SRPTag);


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
	uint32_t size = 0;
	size = 3;
	if(type == 2){
		size = size + 4;
	}
    return size;
}

void SRPTag::Serialize (TagBuffer i) const{    
  i.WriteU8(type);
  i.WriteU16(node);
  if(type == 2){
      i.WriteU32(index);
  		/*i.WriteU16(lsa_size);
  		for(int k=0;k<(int)lsa_size;k++){
  			i.WriteU16(lsa[k]);
  		}*/
  }
}

void SRPTag::Deserialize (TagBuffer i){
    type = i.ReadU8();
    node = i.ReadU16();
    if(type == 2){
      index = i.ReadU32();
    	/*lsa_size = i.ReadU16();
    	lsa.clear();
    	for(int k=0;k<(int)lsa_size;k++){
  			lsa.push_back(i.ReadU16());
  		}*/
    }
}

void SRPTag::Print (std::ostream &os) const{
    os << endl;
}
