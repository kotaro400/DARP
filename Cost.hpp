#pragma once

using namespace std;
#include <string>
#include <vector>

class RouteList;

class Cost{
private:
  vector<vector<double> > cost;

public:
  Cost(int RequestSize);
  void setCost(int Xindex,double Xlat,double Xlng,int Yindex,double Ylat,double Ylng);
  double getCost(int indexI,int indexJ);
  double CalcDistance(RouteList *routelist);
};
