using namespace std;
#include <iostream>
#include <random>
#include <cmath>

int main(){
  mt19937_64 mt64(1);

  for(int i = 0; i < 10; i++){
    cout << abs((int)mt64()) % 100 << endl;
  }

  return 0;
}
