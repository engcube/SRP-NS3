#ifndef SRP_TAG_H
#define SRP_TAG_H

#include "ns3/tag.h"
#include "ns3/uinteger.h"
#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

namespace ns3{

class SRPTag : public Tag
{
public:
   SRPTag(){this->type = 0;};
   static TypeId GetTypeId (void);
   virtual TypeId GetInstanceTypeId (void) const;
   virtual uint32_t GetSerializedSize (void) const;
   virtual void Serialize (TagBuffer i) const;
   virtual void Deserialize (TagBuffer i);
   virtual void Print (std::ostream &os) const;

   // these are our accessors to our tag structure

   void setType(uint8_t type){this->type = type;};
   uint8_t getType(){return this->type;};

   void setNode(uint16_t node){this->node = node;};
   uint16_t getNode(){return this->node;};  

   void setLSA(uint32_t index){
      this->index = index;
   };

   uint32_t getLSAIndex(){
      return this->index;
   };

private:
   //1 -- hello
   //2 -- LSA update
   uint8_t type;
   uint16_t node;
   uint32_t index;
};

}
#endif
