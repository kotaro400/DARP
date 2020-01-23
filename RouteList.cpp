#include "RouteList.hpp"
using namespace std;
#include <iostream>
#include <random>
#include <cmath>
#include <tuple>
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

tuple<int, int,int,int> RouteList::OuterRouteChange_random(int customerSize){
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

    // auto it = this->Routelist[afterindex].end()-1;
    // it = this->Routelist[afterindex].insert(it,first);
    // it++;
    // it = this->Routelist[afterindex].insert(it,second);
    // it++;
    return make_tuple(beforeindex,afterindex,first,second);
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

void RouteList::InnerOrderChange_node(int customerSize,int RouteIndex){
    int index,firstindex,first,another,anotherindex,newindex;
    bool ispick;
    mt19937_64 mt64(rand());
    index = RouteIndex;
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

tuple<int, int> RouteList::OuterRouteChange_specified(int customerSize,int worstRouteIndex){
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
    auto it = this->Routelist[afterindex].end()-1;
    it = this->Routelist[afterindex].insert(it,first);
    it++;
    it = this->Routelist[afterindex].insert(it,second);
    it++;
    cout << beforeindex << "から" << afterindex << endl;
    return make_tuple(beforeindex,afterindex);
}

tuple<int, int> RouteList::OuterRouteChange_specified_double(int customerSize,int worstRouteIndex,int bestRouteIndex){
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
    afterindex = bestRouteIndex;
    auto it = this->Routelist[afterindex].end()-1;
    it = this->Routelist[afterindex].insert(it,first);
    it++;
    it = this->Routelist[afterindex].insert(it,second);
    it++;
    cout << beforeindex << "から" << afterindex << " specified double"<<endl;
    return make_tuple(beforeindex,afterindex);
}



tuple<int, int> RouteList::OuterRouteChange_worstNode(int customerSize,int worstNode,int bestRouteIndex){
    cout <<  "outer worstnode 指定" << endl;
    mt19937_64 mt64(rand());
    int first,second,beforeindex,afterindex,firstindex;
    first = worstNode;
    if (first <= customerSize){
        second = first + customerSize;
    }else{
        second = first;
        first = second -customerSize;
    }
    cout << first << " " << second << endl;
    for (int i=0;i<this->Routelist.size();i++){
        for(auto itr=this->Routelist[i].begin();itr!=this->Routelist[i].end();){
            if (*itr == second || *itr == first){
                itr = this->Routelist[i].erase(itr);
                beforeindex = i;
            }else{
                ++itr;
            }
        }
    }
    afterindex = bestRouteIndex;
    auto it = this->Routelist[afterindex].end()-1;
    it = this->Routelist[afterindex].insert(it,first);
    it++;
    it = this->Routelist[afterindex].insert(it,second);
    it++;
    cout << beforeindex << "から" << afterindex << " worstnode指定"<<endl;
    return make_tuple(beforeindex,afterindex);
}

tuple<int, int,int,int,int,int> RouteList::swapRoute(int customerSize){
    mt19937_64 mt64(rand());
    int beforeindex,afterindex,before_first,before_second,after_first,after_second;
    beforeindex = abs((int)mt64())%this->Routelist.size();
    int before_first_index = abs((int)mt64())%(this->Routelist[beforeindex].size()-2)+1;
    before_first = this->Routelist[beforeindex][before_first_index];
    if (before_first <= customerSize){
        before_second = before_first + customerSize;
    }else{
        before_second = before_first;
        before_first = before_second -customerSize;
    }
    for(auto itr=this->Routelist[beforeindex].begin();itr!=this->Routelist[beforeindex].end();){
        if (*itr == before_second || *itr == before_first){
            itr = this->Routelist[beforeindex].erase(itr);
        }else{
            ++itr;
        }
    }
    // cout << beforeindex << "から" << before_first << "と" << before_second << "を削除" << endl;
    afterindex = abs((int)mt64())%this->Routelist.size();
    while(beforeindex == afterindex){
        afterindex = abs((int)mt64())%this->Routelist.size();
    }
    int after_first_index = abs((int)mt64())%(this->Routelist[afterindex].size()-2)+1;
    after_first = this->Routelist[afterindex][after_first_index];
    if (after_first <= customerSize){
        after_second = after_first + customerSize;
    }else{
        after_second = after_first;
        after_first = after_second -customerSize;
    }
    for(auto itr=this->Routelist[afterindex].begin();itr!=this->Routelist[afterindex].end();){
        if (*itr == after_second || *itr == after_first){
            itr = this->Routelist[afterindex].erase(itr);
        }else{
            ++itr;
        }
    }
    // cout << afterindex << "から" << after_first << "と" << after_second << "を削除" << endl;
    //  2つのリクエストペアを削除

    // 挿入
    // beforeindexの2つをafterindexに挿入
    // auto it = this->Routelist[afterindex].end()-1;
    // it = this->Routelist[afterindex].insert(it,before_first);
    // it++;
    // it = this->Routelist[afterindex].insert(it,before_second);
    
    // afterindexの2つをbeforeindexに挿入
    // it = this->Routelist[beforeindex].end()-1;
    // it = this->Routelist[beforeindex].insert(it,after_first);
    // it++;
    // it = this->Routelist[beforeindex].insert(it,after_second);
    return make_tuple(beforeindex,afterindex,before_first,before_second,after_first,after_second);
}

void RouteList::insertRoute(int routeindex,int place,int number){
    auto it = this->Routelist[routeindex].begin();
    it += place;
    this->Routelist[routeindex].insert(it,number);
}

void removeElement(vector<int> &vector, int index) {
    vector.erase(vector.begin()+index);
}

