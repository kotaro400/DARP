#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <map> 

using namespace std;
void removeElement(vector<int> &vector, int index);
int main(){
    vector<vector<int> > vec;
    vector<int> tmp;
    int first,second;
    vec.push_back(tmp);
    vec.push_back(tmp);
    vec[0].push_back(0);
    vec[0].push_back(1);
    vec[0].push_back(7);
    vec[0].push_back(3);
    vec[0].push_back(9);
    vec[0].push_back(5);
    vec[0].push_back(11);
    vec[0].push_back(0);
    vec[1].push_back(0);
    vec[1].push_back(2);
    vec[1].push_back(8);
    vec[1].push_back(4);
    vec[1].push_back(10);
    vec[1].push_back(6);
    vec[1].push_back(12);
    vec[1].push_back(0);
    for(int i=0;i<vec.size();i++){
        for(int j=0;j<vec[i].size();j++){
            cout << vec[i][j] << " ";
        }
        cout << endl;
    }
    
    mt19937_64 mt64(0);

    int index = abs((int)mt64())%(vec.size());
    cout << "index:" << index << endl;
    int firstindex =abs((int)mt64())%(vec[index].size()-2)+1;
    first = vec[index][firstindex];
    int requestSize=6;
    if (first <= requestSize){ 
        second = first + requestSize;
    }else{
        second = first;
        first = second -requestSize;
    }
    cout <<"first:"<< first << ",second:" << second << endl;
    // firstを削除
    // secondを削除
    for(auto it = vec[index].begin();it!=vec[index].end();){
        if (*it == second || *it == first){
            it = vec[index].erase(it);
        }else{
            ++it;
        }
    }

    for(int i=0;i<vec.size();i++){
        for(int j=0;j<vec[i].size();j++){
            cout << vec[i][j] << " ";
        }
        cout << endl;
    }
    int anotherindex =abs((int)mt64())%(vec.size());
    while(index == anotherindex){
        anotherindex =abs((int)mt64())%(vec.size());
    }
    cout << "anotherindex:" << anotherindex << endl;
    // 挿入
    // random_int = mt64();
    auto it = vec[anotherindex].end()-1;
    // it += 1+2*(abs(random_int)%4);
    it = vec[anotherindex].insert(it, first);
    it++;
    it = vec[anotherindex].insert(it, second);
    it++;
    for(int i=0;i<vec.size();i++){
        for(int j=0;j<vec[i].size();j++){
            cout << vec[i][j] << " ";
        }
        cout << endl;
    }

}

void removeElement(vector<int> &vector, int index) {
    vector.erase(vector.begin()+index);
}
