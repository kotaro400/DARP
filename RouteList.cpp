#include "RouteList.hpp"
using namespace std;
#include <iostream>

RouteList::RouteList(int VehicleNum){
    cout << "RouteListのコンストラクタ" << endl;
    this->VehicleNum = VehicleNum;
    this->Routelist.reserve(VehicleNum);
	this->Routelist.resize(VehicleNum);
}

void RouteList::makeInitialRoute(int RequestSize){
    cout << "初期解生成スタート" << endl;
    int CustomerSize=RequestSize/2;
    int i=1;
    while(i<=CustomerSize){
        int amari=  i % this->VehicleNum;
        if (amari == 0){
            this->Routelist[this->VehicleNum-1].push_back(i);
            this->Routelist[this->VehicleNum-1].push_back(i+CustomerSize);
        }else{
            this->Routelist[amari-1].push_back(i);
            this->Routelist[amari-1].push_back(i+CustomerSize);
        }
        i++;
    }
    cout <<"初期解生成おわり"<< endl;
}

int RouteList::getRouteSize(int number){
    return this->Routelist[number].size();
}

int RouteList::getVehicleNum(){
    return this->VehicleNum;
}

int RouteList::getRoute(int RouteNumber,int RouteOrder){
    return this->Routelist[RouteNumber][RouteOrder];
}
