#include <iostream>
#include <vector>
#include <cmath>
#include <random>

using namespace std;
void removeElement(vector<int> &vector, int index);
int main(){
    vector<int> vec;
    int random_int,first,second;
    for(int i=0;i<9;i++){
        vec.push_back(i);
    }
    vec.push_back(0);
    for(int i=0;i<10;i++){
        std::cout << vec[i] << " ";
    }
    cout << endl;

    mt19937_64 mt64(0);

    for(int i=0;i<10;i++){
    random_int = mt64();

    first = vec.at(1+2*(abs(random_int)%4));
    second = vec.at(1+2*(abs(random_int)%4)+1);
    removeElement(vec,1+2*(abs(random_int)%4));
    removeElement(vec,1+2*(abs(random_int)%4));

    // cout << "削除されたのは," << first << " " << second << endl;
    // for(int i=0;i<vec.size();i++){
    //     std::cout << vec[i] << " ";
    // }
    // cout << endl;
    // cout << "削除後のvec:" << vec.capacity() << endl;


    // 挿入
    random_int = mt64();
    auto it = vec.begin();
    it += 1+2*(abs(random_int)%4);
    it = vec.insert(it, first);
    it++;
    it = vec.insert(it, second);
    it++;

    for(int i=0;i<vec.size();i++){
        std::cout << vec[i] << " ";
    }
    cout << vec.capacity();
    cout << endl;

    }


}

void removeElement(vector<int> &vector, int index) {
    vector.erase(vector.begin()+index);
}
