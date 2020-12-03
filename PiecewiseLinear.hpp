#pragma once

struct Function{
  int lower;
  int upper;
  double slope;
  double intercept;
  Function *next;
};

class PiecewiseLinear{
public:
  Function *head;
  PiecewiseLinear();
  void appendFunction(int lower, int upper, double slope, double intercept);
  void displayFunctions();
  PiecewiseLinear* shiftToRight(int diff);
  PiecewiseLinear* minimize();
  PiecewiseLinear* copy(PiecewiseLinear* origin);
  PiecewiseLinear* sum(PiecewiseLinear* plf_1, PiecewiseLinear* plf_2);
};
