#include "RouteList.hpp"
#include "PiecewiseLinear.hpp"
#include "Cost.hpp"
using namespace std;
#include <iostream>
#include <random>
#include <cmath>
#include <tuple>
void removeElement(vector<int> &vector, int index); //vectorの指定したインデックスの要素を削除する関数

RouteList::RouteList(int VehicleNum, int RequestSize){
    // cout << "RouteListのコンストラクタ" << endl;
  this->VehicleNum = VehicleNum;

  this->Routelist.reserve(VehicleNum);
	this->Routelist.resize(VehicleNum);

  this->start_times.reserve(VehicleNum);
  this->start_times.resize(VehicleNum);

  this->service_ride_times.reserve(VehicleNum);
  this->service_ride_times.resize(VehicleNum);

  this->min_penalties.reserve(VehicleNum);
  this->min_penalties.resize(VehicleNum);

  this->ride_times.reserve(RequestSize);
  this->ride_times.resize(RequestSize);
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

int RouteList::InnerRouteChange_requestSet(){
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

    // 変更のあった車両を返す
    return index;
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
    mt19937_64 mt64(1);
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




int RouteList::Outer_Relocate(int n,int m, int number){
    int beforeindex;
    for (int VehicleNum = 0;VehicleNum<m;VehicleNum++){
        for (int j = 0;j<this->Routelist[VehicleNum].size();j++){
            if (this->Routelist[VehicleNum][j] == number){
                beforeindex = VehicleNum;
            }
        }
    }
    for (auto itr = this->Routelist[beforeindex].begin();itr != this->Routelist[beforeindex].end();){
        if (*itr == number || *itr == (number+n)){
            itr = this->Routelist[beforeindex].erase(itr);
        }else{
            ++itr;
        }
    }
    return beforeindex;
}

tuple<int,int> RouteList::Outer_Swap(int n,int m,int num1,int num2){
    // beforeindexとafterindexを返す
    int beforeindex,afterindex,before_first,before_second,after_first,after_second;
    before_first = num1;
    before_second = num1 + n;
    after_first = num2;
    after_second = num2 + n;
    cout << num1 << " " << num2 << endl;
        // beforeindexとafterindexを見つける
    for (int VehicileNum = 0;VehicileNum<m;VehicileNum++){
        for (int j=0;j<this->Routelist[VehicileNum].size();j++){
            if (this->Routelist[VehicileNum][j]==num1){
                beforeindex = VehicileNum;
            }
            if (this->Routelist[VehicileNum][j]==num2){
                afterindex = VehicileNum;
            }
        }
    }
    cout << beforeindex << " " << afterindex << endl;
    // 要素を削除
    for (auto itr = this->Routelist[beforeindex].begin();itr != this->Routelist[beforeindex].end();){
        if (*itr == before_first || *itr == before_second){
            itr = this->Routelist[beforeindex].erase(itr);
        }else{
            ++itr;
        }
    }
    for (auto itr = this->Routelist[afterindex].begin();itr != this->Routelist[afterindex].end();){
        if (*itr == after_first || *itr == after_second){
            itr = this->Routelist[afterindex].erase(itr);
        }else{
            ++itr;
        }
    }
    return make_tuple(beforeindex,afterindex);
}

void RouteList::ComputeStartTimes(Cost* cost, InputData* inputdata, int i){
  int n=inputdata->getRequestSize()/2;

  if(!service_ride_times[i].size()){
    service_ride_times[i].resize(getRouteSize(i));
    start_times[i].resize(getRouteSize(i));
    min_penalties[i].resize(getRouteSize(i));
  }

  for(int j = 0; j < getRouteSize(i) - 1; j++){
    if(getRoute(i, j) <= n){
      service_ride_times[i][j] = cost->getCost(getRoute(i, j), getRoute(i, j + 1)) + inputdata->getPickupPointer(getRoute(i, j))->servicetime;
    }else{
      service_ride_times[i][j] = cost->getCost(getRoute(i, j), getRoute(i, j + 1)) + inputdata->getDropoffPointer(getRoute(i, j) - n)->servicetime;
    }
  }

  for(int j = 0; j < getRouteSize(i) - 1; j++){
    min_penalties[i][j].head = NULL;
  }


  for(int j = 1; j < getRouteSize(i) - 1; j++){
    if(j == 1){
      PiecewiseLinear depot_penalty;

      depot_penalty.appendFunction(-1000000, 0, -1000000, 0);
      depot_penalty.appendFunction(0, 1440, 0, 0);
      depot_penalty.shiftToRight(cost->getCost(0, getRoute(i, 1)));
      min_penalties[i][1].sum(&depot_penalty, &(inputdata->getPickupPointer(getRoute(i, j))->penalty));
      min_penalties[i][1].minimize();
    }else{
      if(getRoute(i, j) <= n){
        min_penalties[i][j].sum(min_penalties[i][j - 1].shiftToRight(service_ride_times[i][j - 1]), &(inputdata->getPickupPointer(getRoute(i, j))->penalty));
        min_penalties[i][j].minimize();
      }else{
        min_penalties[i][j].sum(min_penalties[i][j - 1].shiftToRight(service_ride_times[i][j - 1]), &(inputdata->getDropoffPointer(getRoute(i, j) - n)->penalty));
        min_penalties[i][j].minimize();
      }
    }
  }

  for(int j = getRouteSize(i) - 2; j > 0; j--){
    Function* tmp_func = min_penalties[i][j].head;
    double opt_time = 0;
    double min_penalty = min_penalties[i][j].head->intercept;
    double tmp_penalty;

    if(j == getRouteSize(i) - 2){
      while(tmp_func){
        tmp_penalty = tmp_func->slope * tmp_func->upper + tmp_func->intercept;
        if(min_penalty > tmp_penalty){
          min_penalty = tmp_penalty;
          opt_time = tmp_func->upper;
        }
        tmp_func = tmp_func->next;
      }
    }else{
      while(tmp_func){
        double upper_bound = start_times[i][j + 1] - service_ride_times[i][j];
        tmp_penalty = tmp_func->slope * tmp_func->upper + tmp_func->intercept;
        if(min_penalty > tmp_penalty){
          if(tmp_func->upper <= upper_bound){
            min_penalty = tmp_penalty;
            opt_time = tmp_func->upper;
          }else if(tmp_func->slope * upper_bound + tmp_func->intercept < min_penalty){
            min_penalty = tmp_func->slope * upper_bound + tmp_func->intercept;
            opt_time = upper_bound;
          }
        }
        tmp_func = tmp_func->next;
      }
    }
    start_times[i][j] = opt_time;

    if(getRoute(i, j) > n){
      PiecewiseLinear rideTimeFunction;
      double bound = opt_time - inputdata->getDropoffPointer(getRoute(i, j) - n)->servicetime - inputdata->getPickupPointer(getRoute(i, j) - n)->maxRideTime;
      if(bound >= 0){
        rideTimeFunction.appendFunction(0, bound, - 1, bound);
        rideTimeFunction.appendFunction(bound, 1440, 0, 0);
      }else{
        rideTimeFunction.appendFunction(0, 1440, 0, 0);
      }
      int pickupPos = j - 1;
      while (true) {
        if(getRoute(i, pickupPos) == getRoute(i, j) - n){
          break;
        }else{
          pickupPos -= 1;
        }
      }
      for(int k = pickupPos; k < j; k++){
        min_penalties[i][k].head = NULL;
        PiecewiseLinear newPlf;
        if(k == 1){
          PiecewiseLinear depot_penalty;

          depot_penalty.appendFunction(-1000000, 0, -1000000, 0);
          depot_penalty.appendFunction(0, 1440, 0, 0);
          depot_penalty.shiftToRight(cost->getCost(0, getRoute(i, 1)));
          min_penalties[i][1].sum(&depot_penalty, &(inputdata->getPickupPointer(getRoute(i, k))->penalty));
          newPlf.sum(&(min_penalties[i][1]), &rideTimeFunction);
          min_penalties[i][1] = newPlf;
          min_penalties[i][1].minimize();
        }else{
          if(getRoute(i, k) <= n){
            min_penalties[i][k].sum(min_penalties[i][k - 1].shiftToRight(service_ride_times[i][k - 1]), &(inputdata->getPickupPointer(getRoute(i, k))->penalty));
            newPlf.sum(&(min_penalties[i][k]), &rideTimeFunction);
            min_penalties[i][1] = newPlf;
            min_penalties[i][k].minimize();
          }else{
            min_penalties[i][k].sum(min_penalties[i][k - 1].shiftToRight(service_ride_times[i][k - 1]), &(inputdata->getDropoffPointer(getRoute(i, k) - n)->penalty));
            newPlf.sum(&(min_penalties[i][k]), &rideTimeFunction);
            min_penalties[i][1] = newPlf;
            min_penalties[i][k].minimize();
          }
        }
      }
    }else{
      int dropoffPos = j + 1;
      while (true) {
        if(getRoute(i, dropoffPos) == getRoute(i, j) + n){
          break;
        }else{
          dropoffPos += 1;
        }
      }
      ride_times[getRoute(i, j) - 1] = start_times[i][dropoffPos] - opt_time - inputdata->getPickupPointer(getRoute(i, j))->servicetime;
    }
  }
}

bool RouteList::satisfyCapacityConstraint(int routeindex, InputData* inputdata){
  int total_demand = 0;
  int order = 1;
  int n=inputdata->getRequestSize()/2;

  while(order < getRouteSize(routeindex) - 1){
    if(getRoute(routeindex, order) <= n){
      total_demand += 1;
    }else{
      total_demand -= 1;
    }

    if(total_demand <= inputdata->getVehicleCapacity()){
      order += 1;
    }else{
      break;
    }
  }

  if(order == getRouteSize(routeindex) - 1){
    return true;
  }else{
    return false;
  }
}
