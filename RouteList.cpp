#include "RouteList.hpp"
using namespace std;
#include <iostream>
#include <random>
#include <cmath>
void removeElement(vector<int> &vector, int index); //vectorの指定したインデックスの要素を削除する関数

RouteList::RouteList(int VehicleNum){
    // cout << "RouteListのコンストラクタ" << endl;
    this->VehicleNum = VehicleNum;
    this->Routelist.reserve(VehicleNum);
	this->Routelist.resize(VehicleNum);
}

RouteList::~RouteList(){
    // cout  << "RouteListのデストラクタ" << endl;
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
    int RequestSize = (this->Routelist[index].size()-2) / 2;
    int beforeIndex = abs((int)mt64()) % RequestSize;
    int first = this->Routelist[index].at(1+2*(beforeIndex));
    int second = this->Routelist[index].at(1+2*(beforeIndex)+1);
    removeElement(this->Routelist[index],1+2*(beforeIndex));
    removeElement(this->Routelist[index],1+2*(beforeIndex));

    // 挿入
    int afterIndex = abs((int)mt64()) % RequestSize;
    while(beforeIndex == afterIndex){
        afterIndex = abs((int)mt64()) % RequestSize;
        // 変える前と変える後が一緒の場合はやり直す
    }
    auto it = this->Routelist[index].begin();
    it += (1+2*afterIndex);
    it = this->Routelist[index].insert(it,first);
    it++;it = this->Routelist[index].insert(it,second);
    it++;
}

void RouteList::InnerRouteChange_node(int customerSize){
    int index,firstindex,first,another,anotherindex,newindex;
    bool ispick;
    mt19937_64 mt64(rand());
    index= abs((int)mt64()) % this->Routelist.size();
    int RequestSize_inRoot = (this->Routelist[index].size()-2) / 2;
    firstindex = abs((int)mt64())%(this->Routelist[index].size()-2)+1;
    first = this->Routelist[index][firstindex];
    if (first <= customerSize){
        ispick = true;
        another = first + customerSize;
    }else{
        ispick = false;
        another = first - customerSize;
    }
    for(auto it = this->Routelist[index].begin();it!=this->Routelist[index].end();++it){
        if (*it == another){
            anotherindex =distance(this->Routelist[index].begin(),it);
        }
    }
    removeElement(this->Routelist[index],firstindex);

    // 挿入
    auto it = this->Routelist[index].begin();
    if(ispick){
        newindex = abs((int)mt64())%(anotherindex-1)+1;
        if (newindex == firstindex){
            newindex = abs((int)mt64())%(anotherindex-1)+1;
        }
    }else{
        newindex = abs((int)mt64())%(2*RequestSize_inRoot -anotherindex) + (anotherindex+1);
        if (newindex == firstindex){
            newindex = abs((int)mt64())%(2*RequestSize_inRoot -anotherindex) + (anotherindex+1);
        }
    }
    it += newindex;
    this->Routelist[index].insert(it,first);
}

void RouteList::OuterRouteChange_random(int customerSize){
    int first,second,beforeindex,afterindex,firstindex;
    mt19937_64 mt64(rand());
    beforeindex = abs((int)mt64())%this->Routelist.size();
    firstindex =abs((int)mt64())%(this->Routelist[beforeindex].size()-2)+1;
    first = this->Routelist[beforeindex][firstindex];
    if (first <= customerSize){
        second = first + customerSize;
    }else{
        second = first;
        first = second -customerSize;
    }
    for(auto itr=this->Routelist[beforeindex].begin();itr!=this->Routelist[beforeindex].end();){
        if (*itr == second || *itr == first){
            itr = this->Routelist[beforeindex].erase(itr);
        }else{
            ++itr;
        }
    }
    afterindex = abs((int)mt64())%this->Routelist.size();
    while (beforeindex == afterindex){
        afterindex = abs((int)mt64())%this->Routelist.size();
    }

    auto it = this->Routelist[afterindex].end()-1;
    it = this->Routelist[afterindex].insert(it,first);
    it++;
    it = this->Routelist[afterindex].insert(it,second);
    it++;
}


void RouteList::InnerRouteChange_specified(int customerSize,int worst){
    int index,worstindex,anotherindex,another;
    bool ispick;
    mt19937_64 mt64(rand());
    if (worst <= customerSize){
        ispick = true;
        another = worst + customerSize;
    }else{
        ispick = false;
        another = worst - customerSize;
    }
    // cout << "worst: " << worst << " another: " << another << endl;
    for (int i=0;i<this->Routelist.size();i++){
        for (int j=0;j<this->Routelist[i].size();j++){
            if (this->Routelist[i][j] == worst){
                worstindex = j;
                index = i;
            }else if(this->Routelist[i][j] == another){
                anotherindex = j;
            }
        }
    }
    // cout << "i:" << index << " worstidnex: " << worstindex << " anotherindex: " << anotherindex << endl;
    int requestSize_inRoot = (this->Routelist[index].size()-2)/2;
    // cout << "リクエストの数: " << requestSize_inRoot << endl;
    removeElement(this->Routelist[index],worstindex);
    int newindex;
    auto it = this->Routelist[index].begin();

    if (ispick){
        newindex = abs((int)mt64())%(anotherindex-1)+1;
        if (newindex == worstindex){
            newindex = abs((int)mt64())%(anotherindex-1)+1;
        }
    }else{
        newindex = abs((int)mt64())%(2*requestSize_inRoot -anotherindex) + (anotherindex+1);
        if (newindex == worstindex){
            newindex = abs((int)mt64())%(2*requestSize_inRoot -anotherindex) + (anotherindex+1);
        }
    }
    // cout  << "newindex:" << newindex << endl;
    it += newindex;  
    this->Routelist[index].insert(it,worst);
    // cout << "------------------" << endl;
    // for(int i=0;i<this->Routelist.size();i++){
    //     for (int j=0;j<this->Routelist[i].size();j++){
    //         cout << this->Routelist[i][j] << " ";
    //     }
    //     cout << endl;
    // }
}

void RouteList::InnerOrderChange_requestset(int RouteIndex){
    mt19937_64 mt64(rand());
    int RequestSize = (this->Routelist[RouteIndex].size()-2) / 2;
    int beforeIndex = abs((int)mt64()) % RequestSize;
    int first = this->Routelist[RouteIndex].at(1+2*(beforeIndex));
    int second = this->Routelist[RouteIndex].at(1+2*(beforeIndex)+1);
    removeElement(this->Routelist[RouteIndex],1+2*(beforeIndex));
    removeElement(this->Routelist[RouteIndex],1+2*(beforeIndex));

     // 挿入
     int afterIndex = abs((int)mt64()) % RequestSize;
     while(beforeIndex == afterIndex){
        afterIndex = abs((int)mt64()) % RequestSize;
        // 変える前と変える後が一緒の場合はやり直す
    }
    auto it = this->Routelist[RouteIndex].begin();
    it += (1+2*afterIndex);
    it = this->Routelist[RouteIndex].insert(it,first);
    it++;
    it = this->Routelist[RouteIndex].insert(it,second);
}

void RouteList::OuterRouteChange_specified(int customerSize,int worstRouteIndex){
    cout << "OuterROuteCHange_Specified" << endl;
    mt19937_64 mt64(rand());
    int first,second,beforeindex,afterindex,firstindex;
    beforeindex = worstRouteIndex;
    firstindex =abs((int)mt64())%(this->Routelist[beforeindex].size()-2)+1;
    first = this->Routelist[beforeindex][firstindex];
    if (first <= customerSize){
        second = first + customerSize;
    }else{
        second = first;
        first = second -customerSize;
    }
    for(auto itr=this->Routelist[beforeindex].begin();itr!=this->Routelist[beforeindex].end();){
        if (*itr == second || *itr == first){
            itr = this->Routelist[beforeindex].erase(itr);
        }else{
            ++itr;
        }
    }
    afterindex = abs((int)mt64())%this->Routelist.size();
    while (beforeindex == afterindex){
        afterindex = abs((int)mt64())%this->Routelist.size();
    }
    cout << "afterindex: " << afterindex << endl;
    auto it = this->Routelist[afterindex].end()-1;
    it = this->Routelist[afterindex].insert(it,first);
    it++;
    it = this->Routelist[afterindex].insert(it,second);
    it++;
    cout << beforeindex << "から" << afterindex << endl;
}

void removeElement(vector<int> &vector, int index) {
    vector.erase(vector.begin()+index);
}

