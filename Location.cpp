using namespace std;
#include"Location.hpp"
#include <string>
#include <iostream>

Location::Location(){
	// cout << "Locationのコンストラクタ" << endl;
}
void Location::setLocation(const int index, const double lat, const double lng) {
	this->index = index;
	this->lat = lat;
	this->lng = lng;
}

int Location::getIndex() const {
	return this->index;
}

double Location::getLat() const {
	return this->lat;
}

double Location::getLng() const {
	return this->lng;
}