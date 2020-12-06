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
  int LatestArrivalDepotTime;
  vector<Location> locationList; //デポがidx:0, pickが1-n dropがn+1-2n
  vector<Request> RequestList;
  vector<Pickup> PickupList;
  vector<Dropoff> DropoffList;

public:
  void setInputData(string inputfile);
  int getVehicleNum();
  int getRequestSize();
  int getMaximumRouteDuration();
  int getVehicleCapacity();
  int getLatestArrivalDepotTime();
  void setRideTimePenalty(int i,double cost);
  Location* getLocationPointer(int index);
  Pickup* getPickupPointer(int index);
  Dropoff* getDropoffPointer(int index);
  double getRideInfo(int i);
};
