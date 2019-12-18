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
    int n=3;
    int m=2;
    int i,j;
    string tmp,tmp2,tmp3;
    // vector<string> varlist;
    // for (i=0;i<m;i++){
    //   tmp="t"+to_string(i)+"s";
    //   tmp2="t"+to_string(i)+"e";
    //   tmp3 = "rt"+ to_string(i);
    //   varlist.push_back(tmp);
    //   varlist.push_back(tmp2);
    //   varlist.push_back(tmp3);
    // }

  GRBVar* DepartureTime = 0;
  GRBVar* DepotTime = 0;
  GRBVar* RideTime = 0;

  // ここはおそらく実際も同じ
  double lblist[2*n+1] = {0.0};
  double ublist[2*n+1];
  for(int i=0;i<=2*n;i++) ublist[i]=480.0;
  double objlist[2*n+1];
  for(int i=0;i<=2*n;i++) objlist[i] = 1.0;
  char typelist[2*n+1];
  for(int i=0;i<=2*n;i++) typelist[i] = GRB_CONTINUOUS;
  string DepartureTimeName[2*n+1];
  DepartureTimeName[0] = "depot";
  for(i=1;i<=2*n;i++){
    tmp = "t"+to_string(i);
    DepartureTimeName[i] = tmp;
  }

  // 各ノードの出発時刻の変数を追加
  DepartureTime = model.addVars(lblist,ublist,objlist,typelist,DepartureTimeName,2*n+1);


    // Create variables デポの出発時刻と乗車時間
    GRBVar t0s = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS);
    GRBVar t0e = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "x");
    GRBVar t1s = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "x");
    GRBVar t1e = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "x");
  
    GRBVar rt1 = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "rt1");
    GRBVar rt2 = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "rt2");
    GRBVar rt3 = model.addVar(0.0, 480.0,1.0, GRB_CONTINUOUS, "rt3");

    // ここから区分線形関数を設定
    vector<double> x = {0.0,1.0,3.0,4.0};
    vector<double> y = {5.0,0.0,0.0,4.0};
    
    vector<vector<double> > xvec; //xvecのsizeは2n+1 中のvectorのサイズは可変
    vector<vector<double> > yvec;
    xvec.push_back({0.0}); //ダミー index0は使わない
    yvec.push_back({0.0}); //ダミー
    for(int i=1;i<=2*n;i++){
      xvec.push_back(x); // いまはとりあえず同じvectorを追加
      yvec.push_back(y); // いまはとりあえず同じvectorを追加
    } 
    // ここまでは受け取り

    // ここから区分線形関数を追加
    for (i=1;i<=2*n;i++){
      // 区分線形関数を設定
      double *xpointer;
      double *ypointer;
      xpointer = new double[xvec[i].size()];
      ypointer = new double[yvec[i].size()];
      for(int j=0;j<xvec[i].size();j++){
        xpointer[j] = xvec[i][j];
        ypointer[j] = yvec[i][j];
      }
      model.setPWLObj(DepartureTime[i],4,xpointer,ypointer);
      delete[] xpointer;
      delete[] ypointer;
    }
// ここまで区分線形関数を追加

//目的関数を追加
    GRBLinExpr siki;
    siki = DepartureTime[1] + DepartureTime[2] + DepartureTime[3] + DepartureTime[4] + DepartureTime[5] + DepartureTime[6] + rt1 + rt2 + rt3;
    // model.setObjective(DepartureTime[1] + DepartureTime[2] + t3 + DepartureTime[4] + DepartureTime[5] + DepartureTime[6] + rt1 + rt2 + rt3, GRB_MINIMIZE);
     model.setObjective(siki, GRB_MINIMIZE);

    // 制約を追加

    // GRBTempConstr c1 = t0s + 1.0 <= DepartureTime[1]; これまちが
    // model.addConstr(c1,"c1");
    GRBTempConstr tempconstr;
    string constrname;
    for (int i=1;i<2*n;i++){
      tempconstr = DepartureTime[i] + 1.0 <= DepartureTime[i+1];
      constrname = "constr"+to_string(i);
      model.addConstr(tempconstr,constrname);
    }
    model.addConstr(rt1 == DepartureTime[4]-DepartureTime[1], "c0");
    model.addConstr(rt2 == DepartureTime[5]-DepartureTime[2], "c1");
    model.addConstr(rt3 == DepartureTime[6]-DepartureTime[3], "c2");
    // Optimize model
    model.set(GRB_IntParam_OutputFlag, 0); //ログの出力をoff
    model.optimize();

     cout << DepartureTime[1].get(GRB_StringAttr_VarName) << " "
         << DepartureTime[1].get(GRB_DoubleAttr_X) << endl;
     cout << DepartureTime[2].get(GRB_StringAttr_VarName) << " "
         << DepartureTime[2].get(GRB_DoubleAttr_X) << endl;
     cout << DepartureTime[3].get(GRB_StringAttr_VarName) << " "
         << DepartureTime[3].get(GRB_DoubleAttr_X) << endl;
     cout << DepartureTime[4].get(GRB_StringAttr_VarName) << " "
         << DepartureTime[4].get(GRB_DoubleAttr_X) << endl;
     cout << DepartureTime[5].get(GRB_StringAttr_VarName) << " "
         << DepartureTime[5].get(GRB_DoubleAttr_X) << endl;
     cout << DepartureTime[6].get(GRB_StringAttr_VarName) << " "
         << DepartureTime[6].get(GRB_DoubleAttr_X) << endl;

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
