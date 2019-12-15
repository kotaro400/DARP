#include "Solution.hpp"
using namespace std;
#include <iostream>

Solution::Solution(RouteList* routelist){
    cout << "solutionのコンストラクタ" << endl;
    this->routelist = routelist;
}

int Solution::test() {
    cout << "solution::test" << endl;
    cout << this->routelist->getRouteSize(2) << endl;
    return 1;
}