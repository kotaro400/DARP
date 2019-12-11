using namespace std;
#include"Cost.hpp"
#include <string>
#include <iostream>
#include <cmath>

Cost::Cost(int RequestSize){
	cout << "Costのコンストラクタ" << endl;
	this->cost.reserve(RequestSize+1);
	this->cost.resize(RequestSize+1);
	for(int i=0;i<=RequestSize;i++){
		this->cost[i].reserve(RequestSize+1);
		this->cost[i].resize(RequestSize+1);
	}
}

void Cost::setCost(int indexI,double latI,double lngI,int indexJ,double latJ,double lngJ){
	double distance;
	distance = sqrt(pow(latI-latJ,2)+pow(lngI-lngJ,2));
	// cout << "i:" << indexI << " j:" << indexJ << " distance:" << distance << endl;
	this->cost[indexI][indexJ]=distance;
}

double Cost::getCost(int indexI,int indexJ){
	return this->cost[indexI][indexJ];	
}

