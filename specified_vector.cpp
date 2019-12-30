#include <iostream>
#include <vector>
#include <random>

using namespace std;


void removeElement(vector<int> &vector, int index);

void removeElement(vector<int> &vector, int index) {
    vector.erase(vector.begin()+index);
}

int main(){
    cout << "tes" << endl;
    vector<vector<int> > vec;
    vector<int> temp;
    vec.push_back(temp);
    vec.push_back(temp);
    vec[0].push_back(0);
    vec[0].push_back(1);
    vec[0].push_back(6);
    vec[0].push_back(2);
    vec[0].push_back(3);
    vec[0].push_back(8);
    vec[0].push_back(7);
    vec[0].push_back(0);
    vec[1].push_back(0);
    vec[1].push_back(4);
    vec[1].push_back(9);
    vec[1].push_back(5);
    vec[1].push_back(10);
    vec[1].push_back(0);
    for(int i=0;i<vec.size();i++){
        for (int j=0;j<vec[i].size();j++){
            cout << vec[i][j] << " ";
        }
        cout << endl;
    }

    for(int k=0;k<100;k++){

    int worst;
    worst = rand()%10+1;
    // worst = 7;
    cout << "worst: " << worst << endl;
    bool ispick;
    int another;
    int customerSize = 5;
    if (worst <= customerSize){
        ispick=true;
        another = worst+customerSize;
    }else{
        ispick = false;
        another = worst - customerSize;
    }
    int index; //ルートのインデックス
    int worstindex,anotherindex;
    for(int i=0;i<vec.size();i++){
        for(int j=0;j<vec[i].size();j++){
            if (vec[i][j]==worst){
                worstindex = j;
                index = i;
            }else if (vec[i][j] == another){
                anotherindex = j;
            }
        }
    }

    int requestSize_inRoot = (vec[index].size()-2)/2;
    
    removeElement(vec[index],worstindex);
    int newindex;

    auto it = vec[index].begin();
    if (ispick){
        newindex = abs(rand())%(anotherindex-1) +1;
        if (worstindex==newindex){
            newindex = abs(rand())%(anotherindex-1) +1;
        }
    }else{
        newindex = abs(rand())%(2*requestSize_inRoot-anotherindex)+(anotherindex+1);
        if (worstindex == newindex){
            newindex = abs(rand())%(2*requestSize_inRoot-anotherindex)+(anotherindex+1);
        }
        
    }
    cout <<"worstindex: " << worstindex << " newindex: " <<  newindex << endl;
    it += newindex;
    vec[index].insert(it,worst);
    for(int i=0;i<vec.size();i++){
        for (int j=0;j<vec[i].size();j++){
            cout << vec[i][j] << " ";
        }
        cout << endl;
    }

    }
}
