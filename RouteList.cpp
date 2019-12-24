#include "RouteList.hpp"
using namespace std;
#include <iostream>
#include <random>
#include <cmath>
void removeElement(vector<int> &vector, int index); //vectorの指定したインデックスの要素を削除する関数

RouteList::RouteList(int VehicleNum){
    cout << "RouteListのコンストラクタ" << endl;
    this->VehicleNum = VehicleNum;
    this->Routelist.reserve(VehicleNum);
	this->Routelist.resize(VehicleNum);
}

RouteList::~RouteList(){
    cout  << "RouteListのデストラクタ" << endl;
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

void RouteList::InnerRouteChange_requestSet(){
    mt19937_64 mt64(rand());
    int index= abs((int)mt64()) % this->Routelist.size();
    cout << "index:" << index << endl;
    int RequestSize = (this->Routelist[index].size()-2) / 2;
    int beforeIndex = abs((int)mt64()) % RequestSize;
    cout << "beforeindex:" << beforeIndex << endl;
    int first = this->Routelist[index].at(1+2*(beforeIndex));
    int second = this->Routelist[index].at(1+2*(beforeIndex)+1);
    cout << first << " " << second << endl;
    removeElement(this->Routelist[index],1+2*(beforeIndex));
    removeElement(this->Routelist[index],1+2*(beforeIndex));

    // 挿入
    int afterIndex = abs((int)mt64()) % RequestSize;
    cout << "afterindex: "<< afterIndex << endl;
    auto it = this->Routelist[index].begin();
    it += (1+2*afterIndex);
    it = this->Routelist[index].insert(it,first);
    it++;it = this->Routelist[index].insert(it,second);
    it++;


}


void removeElement(vector<int> &vector, int index) {
    vector.erase(vector.begin()+index);
}

