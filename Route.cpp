#include "Route.hpp"
using namespace std;
#include <iostream>

Route::Route(){
    cout << "Routeのコンストラクタ" << endl;
}

void Route::setRouteNumber(int num){
    this->RouteNumber = num;
}

// Route* Route::getRouteByNum(int num){
//     return 
// }