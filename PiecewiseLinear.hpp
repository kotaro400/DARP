#pragma once

struct Function{
  double lower;
  double upper;
  double slope;
  double intercept;
  Function *next;
};

class PiecewiseLinear{
public:
  Function *head;
  PiecewiseLinear();
  void appendFunction(double lower, double upper, double slope, double intercept);
  void displayFunctions();
  PiecewiseLinear* shiftToRight(double diff);
  PiecewiseLinear* minimize();
  PiecewiseLinear* copy(PiecewiseLinear* origin);
  PiecewiseLinear* sum(PiecewiseLinear* plf_1, PiecewiseLinear* plf_2);
  double computeValue(double var);
};
