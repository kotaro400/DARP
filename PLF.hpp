//Piecewise linear function
#pragma once

class PLF {
private:
	double  min;			//min value of domain of function
	double  slope;			//slope of function
	double  intercept;		//intercept of function

public:
	void setPLF(double  min, double  slope, double  intercept);
	double  getMin() const;
	double  getSlope() const;
	double  getIntercept() const;
};