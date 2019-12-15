#pragma once

#include <vector>
#include "RouteList.hpp"

class Solution {
private:
    RouteList* routelist;

public:
    Solution(RouteList* routelist);
    int test();
};