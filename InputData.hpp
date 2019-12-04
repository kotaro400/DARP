#pragma once

using namespace std;
#include <string>
#include <vector>
#include"Location.hpp"
#include "Node.hpp"
class InputData{
    private:
        int VehicleNum;
        int UserSize;
        int MaximumRouteDuration;
        int VehicleCapacity;
        vector<Location> LocationList;
        vector<Request> RequestList;
    public:
        void setInputData(string inputfile);
        void setVehicleNum(int vehiclesize);
        void setUserSize(int usersize);
        void setMaximumRouteDuration(int maximumrouteduration);
        void setVehicleCapacity(int vehiclecapacity);
        int getVehicleNum();
        int getUserSize();
        int getMaximumRouteDuration();
        int getVehicleCapacity();


};