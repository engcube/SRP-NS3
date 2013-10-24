#ifndef SRP_TAG_H
#define SRP_TAG_H

#include "ns3/tag.h"
#include "ns3/uinteger.h"
#include <iostream>
#include <vector>

using namespace std;

namespace ns3{

class SRPTag : public Tag
{
public:
   static TypeId GetTypeId (void);
   virtual TypeId GetInstanceTypeId (void) const;
   virtual uint32_t GetSerializedSize (void) const;
   virtual void Serialize (TagBuffer i) const;
   virtual void Deserialize (TagBuffer i);
   virtual void Print (std::ostream &os) const;

   // these are our accessors to our tag structure
   
   void setUpList(vector<int> list);
   vector<int> getUpList();
   void add2UpList(int i);

   void setDownList(vector<int> list);
   vector<int> getDownList();
   void add2DownList(int i);

private:
   vector<int> upList;
   vector<int> downList;
};

}
#endif