#pragma once


using namespace std;
#include <string>
#include <vector>
#include"Location.hpp"
#include "Request.hpp"
class InputData{
    private:
        int VehicleNum;
        int RequestSize;
        int MaximumRouteDuration;
        int VehicleCapacity;
        vector<Location> locationList; //デポがidx:0, pickが1-n dropがn+1-2n
        vector<Request> RequestList;
        vector<Pickup> PickupList;
        vector<Dropoff> DropoffList;
    public:
        void setInputData(string inputfile);
        void setVehicleNum(int vehiclesize);
        void setRequestSize(int usersize);
        void setMaximumRouteDuration(int maximumrouteduration);
        void setVehicleCapacity(int vehiclecapacity);
        int getVehicleNum();
        int getRequestSize();
        int getMaximumRouteDuration();
        int getVehicleCapacity();
        void setRideTimePenalty(int i,vector<double> &temp);
        Location* getLocationPointer(int index);
        double getRideInfo(int i);

};