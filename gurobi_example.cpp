/* Copyright 2019, Gurobi Optimization, LLC */

/* This example formulates and solves the following simple MIP model:

     maximize    x +   y + 2 z
     subject to  x + 2 y + 3 z <= 4
                 x +   y       >= 1
                 x, y, z binary
*/

#include "gurobi_c++.h"
using namespace std;

int
main(int   argc,
     char *argv[])
{
  try {

    // Create an environment
    GRBEnv env = GRBEnv(true);
    env.set("LogFile", "mip1.log");
    env.start();

    // Create an empty model
    GRBModel model = GRBModel(env);

    // Create variables
    GRBVar x[2];
    GRBVar fx[2];
    x[0] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "x0");
    x[1] = model.addVar(0.0, 8.0, 0.0, GRB_CONTINUOUS, "x1");
    fx[0] = model.addVar(0.0, 480.0, 0.0, GRB_CONTINUOUS, "fx0");
    fx[1] = model.addVar(0.0, 480.0, 0.0, GRB_CONTINUOUS, "fy1");
    // GRBVar z = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "z");
    double xd[3] = {0.0,0.5,1.0};
    double yd[3] = {1.0,2.0,1.0};
    // model.setPWLObj(x,3,xd,yd);
    model.addGenConstrPWL(x[0],fx[0],3,xd,yd,"c1");
    double xd2[3] = {4.0,5.0,6.0};
    double yd2[3] = {8.0,9.0,10.0};
    model.addGenConstrPWL(x[1],fx[1],3,xd2,yd2,"c2");


    // Set objective: maximize x + y + 2 z
    model.setObjective(fx[0]+fx[1], GRB_MAXIMIZE);

    // Add constraint: x + 2 y + 3 z <= 4
    // model.addConstr(x + 2 * y + 3 * z <= 4, "c0");

    // Add constraint: x + y >= 1
    // model.addConstr(x + y >= 1, "c1");

    // Optimize model
    model.optimize();

    cout << x[0].get(GRB_StringAttr_VarName) << " "
         << x[0].get(GRB_DoubleAttr_X) << endl;
    cout << fx[0].get(GRB_StringAttr_VarName) << " "
         << fx[0].get(GRB_DoubleAttr_X) << endl;
    cout << x[1].get(GRB_StringAttr_VarName) << " "
         << x[1].get(GRB_DoubleAttr_X) << endl;
    cout << fx[1].get(GRB_StringAttr_VarName) << " "
         << fx[1].get(GRB_DoubleAttr_X) << endl;
    // cout << z.get(GRB_StringAttr_VarName) << " "
    //      << z.get(GRB_DoubleAttr_X) << endl;

    cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << endl;

  } catch(GRBException e) {
    cout << "Error code = " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  } catch(...) {
    cout << "Exception during optimization" << endl;
  }

  return 0;
}
