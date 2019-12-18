using namespace std;
#include <iostream>
#include <vector>
#include"Request.hpp"
#include "PLF.hpp"

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
	vector<double> xvec;
	vector<double> yvec;
	this->pickuppenalty.push_back(xvec);
	this->pickuppenalty.push_back(yvec);

}

void Pickup::setPickupPenalty(const vector<double> &temp){
	cout << "setpickuppena" << endl;
	for(int i=0;i<temp.size();i++){
		if (i%2==0){
			this->pickuppenalty[0].push_back(temp[i]);
		}else{
			this->pickuppenalty[1].push_back(temp[i]);
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


Dropoff::Dropoff(){
	vector<double> xvec;
	vector<double> yvec;
	this->dropoffpenalty.push_back(xvec);
	this->dropoffpenalty.push_back(yvec);
	
}

void Dropoff::setDropOffPenalty(const vector<double> &temp){
	cout << "drop" << endl;
	for(int i=0;i<temp.size();i++){
		if (i%2==0){
			this->dropoffpenalty[0].push_back(temp[i]);
		}else{
			this->dropoffpenalty[1].push_back(temp[i]);
		}
	}
}



void Dropoff::setRidePenalty(vector<double> &temp){
	int i,n;
	if ((int)temp.size()%3 != 0) {
		cout << "始点、傾き、切片のどれかが足りてない" << endl;
		exit(1);
	}
	n = (int)temp.size() / 3;
	PLF temp_plf;
	for (i = 0;i < n;i++) {
		temp_plf.setPLF(temp[3 * i], temp[3 * i + 1], temp[3 * i + 2]);
		this->ridepnal.push_back(temp_plf);
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



double Dropoff::getRidePenalty(){
	return this->ridepnal[1].getIntercept();
}
