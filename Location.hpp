#pragma once

using namespace std;
#include <string>
class Location{
    private:
	int index;	//node no.
	double lat;	//latitude
	double lng;	//longitude

public:
	Location();
	void setLocation(const int index, const double lat, const double lng);
	int getIndex() const;
	double getLat() const;
	double getLng() const;
};
