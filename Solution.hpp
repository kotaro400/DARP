#pragma once

#include <vector>
#include "RouteList.hpp"

class Solution {
private:
    RouteList* routelist;
    // routelistから、LPを解いてtimelistを作成っていう流れかなあ
    // timelistはいらない
    double RouteDistance;
public:
    Solution(RouteList* routelist);
    int test();
    double CalcRouteDistance(RouteList* routelist);
};