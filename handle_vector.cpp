#include <iostream>
#include <vector>
#include <cmath>
#include <random>

using namespace std;
void removeElement(vector<int> &vector, int index);
int main(){
    vector<int> vec;
    for(int i=0;i<9;i++){
        vec.push_back(i);
    }
    vec.push_back(0);
    for(int i=0;i<10;i++){
        std::cout << vec[i] << " ";
    }
    cout << endl;
    cout << "vec:" << vec.capacity() << endl;

    mt19937_64 mt64(0);

    int random_int = mt64();
    cout << abs(random_int)%4 << endl;

    int first = vec.at(1+2*(abs(random_int)%4));
    removeElement(vec,1+2*(abs(random_int)%4));
    int second = vec.at(1+2*(abs(random_int)%4));
    removeElement(vec,1+2*(abs(random_int)%4));

    cout << "削除されたのは," << first << " " << second << endl;
    cout << "-------" << endl;
    for(int i=0;i<vec.size();i++){
        std::cout << vec[i] << " ";
    }
    cout << endl;
    cout << "削除後のvec:" << vec.capacity() << endl;


    // 挿入
    random_int = mt64();
    cout << abs(random_int)%4 << endl;
    auto it = vec.begin();
    it += 1+2*(abs(random_int)%4);
    cout << *it << endl;
    it = vec.insert(it, first);
    it++;
    it = vec.insert(it, second);
    it++;

    cout << "------" << endl;
    for(int i=0;i<vec.size();i++){
        std::cout << vec[i] << " ";
    }
    cout << endl;

    cout << "挿入後のvec:" << vec.capacity() << endl;

}

void removeElement(vector<int> &vector, int index) {
    vector.erase(vector.begin()+index);
}
