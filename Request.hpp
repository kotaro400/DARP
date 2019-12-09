#pragma once
// #include"Node.hpp"

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
};

class Pickup : public Request{
    public:
        Pickup();
        void setPickup(int locationIdx, int servicetime, int demand,int earliest,int latest);

};
class Dropoff :public Request{
    public:
        Dropoff();
        void setDropoff(int locationIdx, int servicetime, int demand,int earliest,int latest);
};
// class Depot :public Request{
// };
