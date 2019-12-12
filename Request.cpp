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
}
void Pickup::setPickPenalty(const vector<double> &temp){
	int i, n;
	if ((int)temp.size()%3 != 0) {
		cerr << "始点、傾き、切片のどれかが足りてない" << endl;
		exit(1);
	}
	n = (int)temp.size() / 3;
	PLF temp_plf;
	for (i = 0;i < n;i++) {
		temp_plf.setPLF(temp[3 * i], temp[3 * i + 1], temp[3 * i + 2]);
		this->pickpnal.push_back(temp_plf);
	}
	// cout << this->pickpnal.size() << endl;
}

int Pickup::getPickPenalty(){
	// return (int)this->pickpnal[0].getSlope();
	return this->pickpnal.size();
	
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
	
}

void Dropoff::setDropPenalty(const vector<double> &temp){
	int i, n;
	if ((int)temp.size()%3 != 0) {
		cerr << "始点、傾き、切片のどれかが足りてない" << endl;
		exit(1);
	}
	n = (int)temp.size() / 3;
	PLF temp_plf;
	for (i = 0;i < n;i++) {
		temp_plf.setPLF(temp[3 * i], temp[3 * i + 1], temp[3 * i + 2]);
	}
	this->droppnal.push_back(temp_plf);
}

void Dropoff::setRidePenalty(vector<double> &temp){
	
}

void Dropoff::setDropoff(int locationIdx, int servicetime, int demand,int earliest,int latest){
	this->locationIdx = locationIdx;
	this->demand = demand;
	this->servicetime = servicetime;
	this->earliest = earliest;
	this->latest = latest;
	this->nodetype = nodetype;
}