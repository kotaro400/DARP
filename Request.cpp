using namespace std;
#include <iostream>
#include <vector>
#include"Request.hpp"

Request::Request(){
	// cout << "Requestのコンストラクタ" << endl;
}
void Request::setRequest(int locationIdx, int servicetime, int demand,int earliest,int latest, int nodetype) {
	this->locationIdx = locationIdx;
	this->demand = demand;
	this->servicetime = servicetime;
	this->earliest = earliest;
	this->latest = latest;
	this->nodetype = nodetype;
}

int Request::getRequestdemand(){
	return demand;
}

Pickup::Pickup(){
}

void Pickup::setPickupPenalty(const vector<double> &temp){
	for(int i=0;i<temp.size();i++){
		if (i%2==0){
			this->pickupXpenalty.push_back(temp[i]);
		}else{
			this->pickupYpenalty.push_back(temp[i]);
		}
	}
}


void Pickup::setPickup(int locationIdx, int servicetime, int demand,int earliest,int latest){
	this->locationIdx = locationIdx;
	this->demand = demand;
	this->servicetime = servicetime;
	this->earliest = earliest;
	this->latest = latest;
	this->nodetype = nodetype;
}

vector<double>* Pickup::getPickupPenaltyX(){
	return &this->pickupXpenalty;
}
double Pickup::getPickupPenaltyXValue(int index){
	return this->pickupXpenalty[index];
}
double Pickup::getPickupPenaltyYValue(int index){
	return this->pickupYpenalty[index];
}



Dropoff::Dropoff(){
}

void Dropoff::setDropOffPenalty(const vector<double> &temp){
	for(int i=0;i<temp.size();i++){
		if (i%2==0){
			this->dropoffXpenalty.push_back(temp[i]);
		}else{
			this->dropoffYpenalty.push_back(temp[i]);
		}
	}
}



void Dropoff::setRidePenalty(vector<double> &temp){
	for(int i=0;i<temp.size();i++){
		if (i%2==0){
			// this->ridetimepenalty[0].push_back(temp[i]);
			this->ritetimeXpenalty.push_back(temp[i]);
		}else{
			// this->ridetimepenalty[1].push_back(temp[i]);
			this->ridetimeYpenalty.push_back(temp[i]);
		}
	}
}

void Dropoff::setDropoff(int locationIdx, int servicetime, int demand,int earliest,int latest){
	this->locationIdx = locationIdx;
	this->demand = demand;
	this->servicetime = servicetime;
	this->earliest = earliest;
	this->latest = latest;
	this->nodetype = nodetype;
}

vector<double>* Dropoff::getDropoffPenaltyX(){
	return &this->dropoffXpenalty;
}
double Dropoff::getDropoffPenaltyXValue(int index){
	return this->dropoffXpenalty[index];
}
double Dropoff::getDropoffPenaltyYValue(int index){
	return this->dropoffYpenalty[index];
}
