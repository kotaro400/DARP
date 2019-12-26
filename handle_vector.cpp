#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <map> 
using namespace std;
void removeElement(vector<int> &vector, int index);
int main(){
    vector<int> vec;
    int random_int,first,another;
    int customerSize = 6;
    bool ispick;
    for(int i=0;i<=12;i++){
        vec.push_back(i);
    }
    vec.push_back(0);
    for(int i=0;i<vec.size();i++){
        std::cout << vec[i] << " ";
    }
    cout << endl;

    mt19937_64 mt64(0);
    map<int,int> mp;

    for(int i=0;i<1000;i++){
    random_int = mt64();
    int firstindex = abs(random_int)%(2*customerSize)+1;
    first = vec[firstindex];
    if (first <= customerSize){
        ispick = true;
        another = first + customerSize;
    }else{
        ispick = false;
        another = first -customerSize;
    }
    int anotherindex;
    for(auto it = vec.begin();it!=vec.end();){
        if(*it == another){
            anotherindex = distance(vec.begin(),it);
            ++it;
         }else{
            ++it;
        }
    }
    removeElement(vec,firstindex);
    auto it = vec.begin();

    // 挿入
    int newindex;
    if (ispick){
        newindex = abs((int)mt64())%(anotherindex-1)+1;
        // cout << "newindex:" << newindex << endl;
        it += newindex;
        vec.insert(it,first);
    }else{
        newindex = abs((int)mt64())%(2*customerSize -anotherindex) + (anotherindex+1);
        it += newindex;
        vec.insert(it,first);
    }

    // cout << "削除されたのは," << first << " " << second << endl;
    // for(int i=0;i<vec.size();i++){
    //     std::cout << vec[i] << " ";
    // }
    // cout << endl;
    // cout << "first: " << first << endl;
    // if (first <= customerSize){
    //     cout << "pick" << endl;
    // }else{
    //     cout << "drop" << endl;
    // }
    // 挿入
    // random_int = mt64();
    // auto it = vec.begin();
    // it += 1+2*(abs(random_int)%4);
    // it = vec.insert(it, first);
    // it++;
    // it = vec.insert(it, another);
    // it++;

    }
    for(int i=0;i<vec.size();i++){
        std::cout << vec[i] << " ";
    }
    cout << endl;

}

void removeElement(vector<int> &vector, int index) {
    vector.erase(vector.begin()+index);
}
