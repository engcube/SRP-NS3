#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <ctime>
using namespace std;

int CORE_NUM = 4;
int TOR_NUM = 512;
int BORDER_NUM = 2;

int m_id = 10;

map<int, int> m_SRPRoutingTable;
map<int, map<int, int> > m_Grid;

int calcSourceInterfaceByNode(int id, int node){
    if (id==node) return 0;
    if(id < CORE_NUM){
        return node-CORE_NUM+1;
    }else if(id < CORE_NUM+TOR_NUM+BORDER_NUM){
        if(node-id == TOR_NUM+BORDER_NUM){
            return CORE_NUM+1;
        }else{
            return node+1;
        }
    }else{
        return 1;
    }
}

void reCalculate(){
    map<int, int> routingTable;
    for(map<int, map<int, int> >::iterator it = m_Grid.begin(); it!=m_Grid.end(); ++it){
        map<int, int> grid_line = it->second;
        for(map<int, int>::iterator it2 = grid_line.begin(); it2!=grid_line.end(); ++it2){
            if(it2->second%2==1){
                routingTable[it->first] = calcSourceInterfaceByNode(m_id, it2->first);
                break;
            }
        }
    }
    if(m_id >=CORE_NUM&& m_id < CORE_NUM+TOR_NUM){
        routingTable[m_id+BORDER_NUM+TOR_NUM] = CORE_NUM+1;
    }
    if(m_id >= CORE_NUM+TOR_NUM+BORDER_NUM){
        for(int i = CORE_NUM+TOR_NUM+BORDER_NUM; i < CORE_NUM+TOR_NUM+BORDER_NUM+TOR_NUM; i++){
            if(i==m_id){
                routingTable[i] = 0;
            }else{
                routingTable[i] = 1;
            }
        }
    }
    m_SRPRoutingTable = routingTable;
}

void initGrid(){
    m_Grid.clear();        
    int add = BORDER_NUM+TOR_NUM;

    if(m_id < CORE_NUM){
        for(int i= CORE_NUM; i<CORE_NUM+TOR_NUM; i++){
            map<int, int> grid_line;
            grid_line[i] = 1;
            m_Grid[add+i] = grid_line;
        }
    }else if(m_id<CORE_NUM+TOR_NUM+BORDER_NUM){
        for(int i= CORE_NUM; i<CORE_NUM+TOR_NUM; i++){
            if(i==m_id){
                continue;
            }
            map<int, int> grid_line;
              for(int j = 0; j<CORE_NUM; j++){
                  grid_line[j] = 1;
              }
            m_Grid[add+i] = grid_line;  
        }        
    }
    //cout << toGridString() << endl;
}

string toGridString(){
    stringstream result;
    result << m_id << "\tGrid:" << endl;
    for(map<int, map<int,int> >::iterator it = m_Grid.begin(); it != m_Grid.end(); ++it){
        result << it->first << ":\t\t";
        for(map<int,int>::iterator it2 = it->second.begin(); it2!=it->second.end(); ++it2){
            result << it2->first << ":" << it2->second << "\t";
        }
        result << endl;
    }
    return result.str();
}


string toString(){
    stringstream result;
    result << m_id << "\tRoutingTable:" << endl;
    for(map<int, int>::iterator it = m_SRPRoutingTable.begin(); it != m_SRPRoutingTable.end(); ++it){
        result << it->first << "\t" << it->second << "\n";
    }
    return result.str();
}

int main(){
	clock_t start,stop;
	//time_t start,stop;
	initGrid();
    //start = time(NULL);
	start = clock();
	reCalculate();
    stop = clock();
    //stop = time(NULL);
    cout << stop-start << endl;
    cout << CLOCKS_PER_SEC << endl;
    cout << (stop-start)*1.0/CLOCKS_PER_SEC << endl;
}