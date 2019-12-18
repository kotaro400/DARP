#pragma once
// #include"Node.hpp"
#include <vector>

class Request{
protected:
	int locationIdx;
	int demand;
    int earliest;
    int latest;
    int servicetime;
    int nodetype; //0:depot 1:pickup 2:dropoff

public:
	Request();
    void setRequest(int locationIdx, int servicetime, int demand,int earliest,int latest,int nodetype);
    int getRequestdemand();
    void setPenalties(int penaltytype, vector<double> &temp);

};

class Pickup : public Request{
    private:
        vector<double> pickupXpenalty;
        vector<double> pickupYpenalty;
    public:
        Pickup();
        void setPickupPenalty(const vector<double> &temp);
        void setPickup(int locationIdx, int servicetime, int demand,int earliest,int latest);
        vector<double>* getPickupPenaltyX();
        double getPickupPenaltyXValue(int index);
        double getPickupPenaltyYValue(int index);


};
class Dropoff :public Request{
    private:
        // vector<vector<double> > dropoffpenalty;
        vector<double> dropoffXpenalty;
        vector<double> dropoffYpenalty;
        vector<vector<double> > ridetimepenalty;
    public:
        Dropoff();
        void setDropOffPenalty(const vector<double> &temp);
        void setRidePenalty(vector<double> &temp);
        void setDropoff(int locationIdx, int servicetime, int demand,int earliest,int latest);
        vector<double>* getDropoffPenaltyX();
        double getDropoffPenaltyXValue(int index);
        double getDropoffPenaltyYValue(int index);
};

