#pragma once
// #include"Node.hpp"
#include <vector>
#include "PiecewiseLinear.hpp"

class Request{
protected:
	int locationIdx;
	int demand;
  int earliest;
  int latest;
  int nodetype; //0:depot 1:pickup 2:dropoff
	// double ridetimeSlope;

public:
	int servicetime;
	double maxRideTime;
	Request();
  void setRequest(int locationIdx, int servicetime, int demand,int earliest,int latest,int nodetype);
  int getRequestdemand();
  void setPenalties(int penaltytype, vector<double> &temp);
};

class Pickup : public Request{
private:

public:
	PiecewiseLinear penalty;
	// PiecewiseLinear penaltyWithRidetime;
  Pickup();
  void setPickupPenalty(const vector<double> &temp);
  void setPickup(int locationIdx, int servicetime, int demand,int earliest,int latest);
  // vector<double>* getPickupPenaltyX();
  // double getPickupPenaltyXValue(int index);
  // double getPickupPenaltyYValue(int index);
	// void setPickupPenaltyWithRidetime();
};

class Dropoff :public Request{
private:

  // vector<double> dropoffXpenalty;
  // vector<double> dropoffYpenalty;
  // vector<double> ridetimeXpenalty;
  // vector<double> ridetimeYpenalty;

public:
	PiecewiseLinear penalty;
	// PiecewiseLinear penaltyWithRidetime;
  Dropoff();
  void setDropOffPenalty(const vector<double> &temp);
  // void setRidePenalty(vector<double> &temp);
  void setDropoff(int locationIdx, int servicetime, int demand,int earliest,int latest);
  // vector<double>* getDropoffPenaltyX();
  // double getDropoffPenaltyXValue(int index);
  // double getDropoffPenaltyYValue(int index);
  // vector<double>* getRidetimePenaltyX();
  // double getRidetimePenaltyXValue(int index);
  // double getRidetimePenaltyYValue(int index);
	// void setDropoffPenaltyWithRidetime(double cost);
};
