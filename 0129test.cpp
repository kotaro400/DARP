#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

int main(){
    // vector<pair<int,int > > NeighborList;
    // for (int i=1;i<=4;i++){
    //     for (int j=i;j<=4;j++){
    //         NeighborList.push_back(make_pair(i,j));
    //     }
    // }
    // cout << NeighborList.size() << endl;
    // for (int i=0;i<NeighborList.size();i++){
    //     cout << NeighborList[i].first << " " << NeighborList[i].second << endl;
    // }
    // random_shuffle(NeighborList.begin(),NeighborList.end());
    // cout<< "---" << endl;
    // for (int i=0;i<NeighborList.size();i++){
    //     cout << NeighborList[i].first << " " << NeighborList[i].second << endl;
    // }
    vector<pair<int,int> > SwapList;
    for (int i=1;i<=5;i++){
        for (int j=i;j<=5;j++){
            if (i != j){
                SwapList.push_back(make_pair(i,j));
            }
        }
    }
    for (int i=0;i<SwapList.size();i++){
        cout << SwapList[i].first << " " << SwapList[i].second  << endl;
    }
}
    

