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
    GRBVar t0s = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS);
    GRBVar t0e = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "x");
    GRBVar t1s = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "x");
    GRBVar t1e = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "x");
    GRBVar t1 = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "t1");
    GRBVar t2 = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "t2");
    GRBVar t3 = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "t3");
    GRBVar t4 = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "t4");
    GRBVar t5 = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "t5");
    GRBVar t6 = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "t6");
    GRBVar rt1 = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "rt1");
    GRBVar rt2 = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "rt2");
    GRBVar rt3 = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "rt3");
    double tes[4] = {0.0,1.0,3.0,4.0};
    double tes2[4] ={5.0,0.0,0.0,4.0};
    model.setPWLObj(t1,4,tes,tes2);
    model.setPWLObj(t2,4,tes,tes2);
    model.setPWLObj(t3,4,tes,tes2);
    model.setPWLObj(t4,4,tes,tes2);
    model.setPWLObj(t5,4,tes,tes2);
    model.setPWLObj(t6,4,tes,tes2);


    model.setObjective(t1 + t2 + t3 + t4 + t5 + t6 + rt1 + rt2 + rt3, GRB_MINIMIZE);

    model.addConstr(t0s + 1.0 <= t1,"1");
    model.addConstr(t1 + 7.0 <= t2,"1");
    model.addConstr(t2 + 1.0 <= t3,"1");
    model.addConstr(t3 + 1.0 <= t4,"1");
    model.addConstr(t4 + 1.0 <= t5,"1");
    model.addConstr(t5 + 1.0 <= t6,"1");
    model.addConstr(rt1 == t4-t1, "c0");
    model.addConstr(rt2 == t5-t2, "c1");
    model.addConstr(rt3 == t6-t3, "c2");
    // Optimize model
    model.optimize();

     cout << t1.get(GRB_StringAttr_VarName) << " "
         << t1.get(GRB_DoubleAttr_X) << endl;
     cout << t2.get(GRB_StringAttr_VarName) << " "
         << t2.get(GRB_DoubleAttr_X) << endl;
     cout << t3.get(GRB_StringAttr_VarName) << " "
         << t3.get(GRB_DoubleAttr_X) << endl;
     cout << t4.get(GRB_StringAttr_VarName) << " "
         << t4.get(GRB_DoubleAttr_X) << endl;
     cout << t5.get(GRB_StringAttr_VarName) << " "
         << t5.get(GRB_DoubleAttr_X) << endl;
     cout << t6.get(GRB_StringAttr_VarName) << " "
         << t6.get(GRB_DoubleAttr_X) << endl;

    cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << endl;

// https://stackoverflow.com/questions/55794691/removing-constraint-from-a-model-in-gurobi-c
// これを使ってルートの順番の制約をremove<->addしたい

  } catch(GRBException e) {
    cout << "Error code = " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  } catch(...) {
    cout << "Exception during optimization" << endl;
  }

  return 0;
}
