using namespace std;
#include <iostream>
#include <vector>
#include "PiecewiseLinear.hpp"

PiecewiseLinear::PiecewiseLinear(){
  head = NULL;
}

void PiecewiseLinear::appendFunction(int lower, int upper, double slope, double intercept){
  Function *new_func;
  Function *tmp;

  new_func = new Function;
  new_func->lower = lower;
  new_func->upper = upper;
  new_func->slope = slope;
  new_func->intercept = intercept;
  new_func->next = NULL;

  if(!head){
    head = new_func;
  }else{
    tmp = head;
    while(tmp->next){
      tmp = tmp->next;
    }
    tmp->next = new_func;
  }
}

void PiecewiseLinear::displayFunctions(){
  if(head == NULL){
    cout << "no function." << endl;
  }else{
    Function* tmp = head;

    while(tmp){
      cout << "lower: " << tmp->lower << endl;
      cout << "upper: " << tmp->upper << endl;
      cout << "slope: " << tmp->slope << endl;
      cout << "intercept: " << tmp->intercept << endl;
      tmp = tmp->next;
    }
  }
}

PiecewiseLinear* PiecewiseLinear::shiftToRight(int diff){
  Function* tmp = this->head;

  while(tmp){
    if(tmp->upper != 480){
      tmp->upper += diff;
    }
    tmp->intercept += -diff * tmp->slope;
    if(tmp->lower != 0){
      tmp->lower += diff;
    }
    tmp = tmp->next;
  }

  return this;
}

PiecewiseLinear* PiecewiseLinear::minimize(){
  Function* tmp = this->head;
  double min;

  while(tmp->slope <= 0){
    tmp = tmp->next;
  }

  min = tmp->slope * tmp->lower + tmp->intercept;

  while(tmp){
    tmp->intercept = min;
    tmp->slope = 0;
    tmp = tmp->next;
  }

  return this;
}

PiecewiseLinear* PiecewiseLinear::copy(PiecewiseLinear* origin){
  Function* ori_plf_func = origin->head;

  while(ori_plf_func){
    appendFunction(ori_plf_func->lower, ori_plf_func->upper, ori_plf_func->slope, ori_plf_func->intercept);
    ori_plf_func = ori_plf_func->next;
  }

  return this;
}

PiecewiseLinear* PiecewiseLinear::sum(PiecewiseLinear* plf_1, PiecewiseLinear* plf_2){
  Function* plf_1_func = plf_1->head;
  Function* plf_2_func = plf_2->head;

  while(plf_1_func || plf_2_func){
    appendFunction(max(plf_1_func->lower, plf_2_func->lower), min(plf_1_func->upper, plf_2_func->upper), plf_1_func->slope + plf_2_func->slope, plf_1_func->intercept + plf_2_func->intercept);

    if(plf_1_func->upper < plf_2_func->upper){
      plf_1_func = plf_1_func->next;
    }else if(plf_1_func->upper > plf_2_func->upper){
      plf_2_func = plf_2_func->next;
    }else{
      plf_1_func = plf_1_func->next;
      plf_2_func = plf_2_func->next;
    }
  }

  return this;
}
