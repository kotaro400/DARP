#include "iostream"
#include <vector>

using namespace std;

int main(){
  vector<vector<int> > vec(3);

  if(vec[0].size()){
    cout << vec[0].size() << endl;
  }else{
    cout << 1 << endl;
  }
}
