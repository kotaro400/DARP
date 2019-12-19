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
    int i;
    cout << "初期解生成スタート" << endl;
    int CustomerSize=RequestSize/2;
    for(i=0;i<this->VehicleNum;i++){
        this->Routelist[i].push_back(0);
    }
    i=1;
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
    for(i=0;i<this->VehicleNum;i++){
        this->Routelist[i].push_back(0);
    }
    cout <<"初期解生成おわり"<< endl;
}

int RouteList::getRouteSize(int number){
    return this->Routelist[number].size();
}

vector<int>* RouteList::getRoutePointerByIndex(int index){
    return &this->Routelist[index];
}

int RouteList::getRouteListSize(){
    return this->Routelist.size();
}

int RouteList::getVehicleNum(){
    return this->VehicleNum;
}

int RouteList::getRoute(int RouteNumber,int RouteOrder){
    return this->Routelist[RouteNumber][RouteOrder];
}
