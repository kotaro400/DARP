#include "RouteList.hpp"
using namespace std;
#include <iostream>

RouteList::RouteList(int VehicleNum){
    cout << "RouteListのコンストラクタ" << endl;
    this->VehicleNum = VehicleNum;
    this->Route.reserve(VehicleNum);
	this->Route.resize(VehicleNum);
}

void RouteList::makeInitialRoute(int RequestSize){
    cout << "初期解生成スタート" << endl;
    int CustomerSize=RequestSize/2;
    int i=1;
    while(i<=CustomerSize){
        int amari=  i % this->VehicleNum;
        if (amari == 0){
            this->Route[this->VehicleNum-1].push_back(i);
            this->Route[this->VehicleNum-1].push_back(i+CustomerSize);
        }else{
            this->Route[amari-1].push_back(i);
            this->Route[amari-1].push_back(i+CustomerSize);
        }
        i++;
    }
    cout <<"初期解生成おわり"<< endl;
}

