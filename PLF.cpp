#include"PLF.hpp"

void PLF::setPLF(double min, double slope, double intercept) {
	this->min = min;
	this->slope = slope;
	this->intercept = intercept;
}

double PLF::getMin() const {
	return this->min;
}

double PLF::getSlope() const {
	return this->slope;
}

double PLF::getIntercept() const {
	return this->intercept;
}