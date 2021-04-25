using namespace std;
#include <iostream>
#include <vector>
#include"Request.hpp"
#include "PiecewiseLinear.hpp"


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
	for(int j = 0; j < temp.size() / 4; j++){
		penalty.appendFunction(temp[j * 4], temp[j * 4 + 1], temp[j * 4 + 2], temp[j * 4 + 3]);
	}
}

// void Pickup::setPickupPenaltyWithRidetime(){
// 	penaltyWithRidetime.copy(&penalty);
// 	Function* tmp_func = penaltyWithRidetime.head;
//
// 	while(tmp_func){
// 		tmp_func->slope -= ridetimeSlope;
// 		tmp_func = tmp_func->next;
// 	}
// }

void Pickup::setPickup(int locationIdx, int servicetime, int demand,int earliest,int latest){
	this->locationIdx = locationIdx;
	this->demand = demand;
	this->servicetime = servicetime;
	this->earliest = earliest;
	this->latest = latest;
	this->nodetype = nodetype;
}

// vector<double>* Pickup::getPickupPenaltyX(){
// 	return &this->pickupXpenalty;
// }
// double Pickup::getPickupPenaltyXValue(int index){
// 	return this->pickupXpenalty[index];
// }
// double Pickup::getPickupPenaltyYValue(int index){
// 	return this->pickupYpenalty[index];
// }



Dropoff::Dropoff(){
}

void Dropoff::setDropOffPenalty(const vector<double> &temp){
	for(int j = 0; j < temp.size() / 4; j++){
		penalty.appendFunction(temp[j * 4], temp[j * 4 + 1], temp[j * 4 + 2], temp[j * 4 + 3]);
	}
}



// void Dropoff::setRidePenalty(vector<double> &temp){
// 	for(int i=0;i<temp.size();i++){
// 		if (i%2==0){
// 			this->ridetimeXpenalty.push_back(temp[i]);
// 		}else{
// 			this->ridetimeYpenalty.push_back(temp[i]);
// 		}
// 	}
// }

void Dropoff::setDropoff(int locationIdx, int servicetime, int demand,int earliest,int latest){
	this->locationIdx = locationIdx;
	this->demand = demand;
	this->servicetime = servicetime;
	this->earliest = earliest;
	this->latest = latest;
	this->nodetype = nodetype;
}

// void Dropoff::setDropoffPenaltyWithRidetime(double cost){
// 	penaltyWithRidetime.copy(&penalty);
// 	Function* tmp_func = penaltyWithRidetime.head;
//
// 	while(tmp_func){
// 		tmp_func->slope += ridetimeSlope;
// 		tmp_func->intercept -= ridetimeSlope * (cost + servicetime);
// 		tmp_func = tmp_func->next;
// 	}
// }


// vector<double>* Dropoff::getDropoffPenaltyX(){
// 	return &this->dropoffXpenalty;
// }
// double Dropoff::getDropoffPenaltyXValue(int index){
// 	return this->dropoffXpenalty[index];
// }
// double Dropoff::getDropoffPenaltyYValue(int index){
// 	return this->dropoffYpenalty[index];
// }
//
// vector<double>* Dropoff::getRidetimePenaltyX(){
// 	return &this->ridetimeXpenalty;
// }
// double Dropoff::getRidetimePenaltyXValue(int index){
// 	return this->ridetimeXpenalty[index];
// }
// double Dropoff::getRidetimePenaltyYValue(int index){
// 	return this->ridetimeYpenalty[index];
// }
