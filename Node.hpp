#pragma once

using namespace std;
#include"Location.hpp"
#include <string>
#include <vector>
class Node {
    public:
        enum class NodeType:int{PICK, DROP, DEPOT};
    private:
        //fixed
        NodeType type;	//Pick(0), Drop(1), Depot(2)
        int index;		//node index (same cost index, location index)
        const Location* location;

        //variable
        int order;					//the order of the node in the route
        Node* pre;
        Node* next;
        int vehicleid;

        //Status
        int cap;			//the number of bording persons after service
        double time;				//service start time value
        

    public:
        Node();

        void setType(const NodeType type);
        void setIndex(const int value);
        void setLocation(const Location* location);

        void setOrder(const int value);
        void setPre(Node* pre);
        void setNext(Node* next);
        
        void initStatus(const int r, const int p);
        void initStatus(const int r, const int p, const double b);
        
        void setCap(const int cap);
        void setTime(const double time);
        

        Node::NodeType getType() const;
        int getIndex() const;
        const Location* getLocation() const;

        int getOrder() const;
        Node* getPre() const;
        Node* getNext() const;
        
        const vector<int>* getCap() const;
        double getTime() const;

        void updateOrder();
};

class Request :public Node {
private:
	//fixed
	int demand;
    int earliest;
    int latest;

public:
	Request();
};


//**********************//
// Pick					//
//**********************//
class Pickup :public Request {
private:
	Node* drop;
public:
	Pickup();

	void setDrop(Node* drop);
	Node* getDrop() const;

	void updateCap();
};

//**********************//
// Drop					//
//**********************//
class Dropoff :public Request {
private:
	Node* pick;
public:
	Dropoff();

	void setPick(Node* pick);
	Node* getPick() const;

	void updateCap();
};

//**********************//
// Depot					//
//**********************//
class Depot :public Node {
private:
	//fixed
	double servicetime;
public:
	Depot();

	void setServicetime(const double value);
	double getServicetime() const;
};