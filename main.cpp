#include <iostream>
#include "InputData.hpp"
#include "Route.hpp"
#include "RouteList.hpp"
#include "Location.hpp"
#include "Cost.hpp"
#include "myfunction.hpp"
#include "Solution.hpp"
#include <gurobi_c++.h>
using namespace std;

int main(int argc, char *argv[]){
    int i,j;
    // 入力を受け取り
    InputData inputdata;
    string inputfile = argv[1];
    inputdata.setInputData(inputfile = argv[1]);
    cout << "車両数:" << inputdata.getVehicleNum() << endl;
    cout << "リクエスト数:" << inputdata.getRequestSize() << endl;
    cout << "ルートの最大長さ:" << inputdata.getMaximumRouteDuration() << endl;
    cout << "車両の容量:" << inputdata.getVehicleCapacity() << endl;

    // 2店間の距離と時間を計算
    Location* loc1;
    Location* loc2;
    Cost cost(inputdata.getRequestSize());
    for (i=0;i<=inputdata.getRequestSize();i++){
        for (j=0;j<=inputdata.getRequestSize();j++){
            // コストを計算
            loc1 = inputdata.getLocationPointer(i);
            loc2 = inputdata.getLocationPointer(j);
            cost.setCost(i,loc1->getLat(),loc1->getLng(),j,loc2->getLat(),loc2->getLng());
        }
    }
    // 乗車時間のペナルティ関数をここでいれる
    for(int i=1;i<=inputdata.getRequestSize()/2;i++){
        // cout << "i:" << i << endl;
        // cout << cost.getCost(i,i+inputdata.getRequestSize()/2) << endl;
        vector<double> vec{ 0.0, 0.0, 0.0,cost.getCost(i,i+inputdata.getRequestSize()/2),5,cost.getCost(i,i+inputdata.getRequestSize()/2)*(-5)};
        inputdata.setRideTimePenalty(i,vec);
        vec.clear();
    }
    // cout << inputdata.getRideInfo(24) << endl;

    // RouteListクラス:複数のルートをまとめて保持するクラス 
    // Routeクラスよりも初期解生成とか近傍探索がしやすそう
    RouteList RouteList(inputdata.getVehicleNum());
    RouteList.makeInitialRoute(inputdata.getRequestSize());
    cout << RouteList.getRouteSize(1) << endl;
    
// クラスでenvとmodelを保持できなさそうだからmainの中で宣言する

    int n=inputdata.getRequestSize()/2;
    int m=inputdata.getVehicleNum();
    try{
        GRBEnv env=GRBEnv(true);
        env.set("LogFile", "mip1.log");
        env.start();
        GRBModel model = GRBModel(env);
        GRBVar* DepartureTime = 0;
        GRBVar* DepotTime = 0;
        GRBVar* RideTime = 0;

        string tmp;
        // 各ノードの出発時刻の変数を追加
        double *lblist; //下限
        lblist = new double[2*n+1];
        for(int i=0;i<=2*n;i++) lblist[i] = 0.0;
        double *ublist; //上限
        ublist = new double[2*n+1];
        for(int i=0;i<=2*n;i++) ublist[i]=inputdata.getMaximumRouteDuration(); 
        double *objlist;
        objlist = new double[2*n+1];//係数(とりあえず1にした)
        for(int i=0;i<=2*n;i++) objlist[i] = 1.0;
        char *typelist;//type
        typelist = new char[2*n+1];
        for(int i=0;i<=2*n;i++) typelist[i] = GRB_CONTINUOUS;
        string *VarName;
        VarName = new string[2*n+1];//名前 ノードiの出発時刻をt_iとする
        VarName[0] = "dummy";
        for(i=1;i<=2*n;i++){
            tmp = "t_"+to_string(i);
            VarName[i] = tmp;
        }
        DepartureTime = model.addVars(lblist,ublist,objlist,typelist,VarName,2*n+1);
        delete[] lblist;
        delete[] ublist;
        delete[] objlist;
        delete[] typelist;
        delete[] VarName;

        // デポの出発と帰る時刻
        lblist = new double[2*m];
        for(int i=0;i<2*m;i++) lblist[i] = 0.0;
        ublist = new double[2*m];
        for(int i=0;i<2*m;i++) ublist[i]=inputdata.getMaximumRouteDuration(); 
        objlist = new double[2*m];//係数(とりあえず1にした)
        for(int i=0;i<2*m;i++) objlist[i] = 1.0;
        typelist = new char[2*m];
        for(int i=0;i<2*m;i++) typelist[i] = GRB_CONTINUOUS;
        VarName = new string[2*m];//名前 ノードiの出発時刻をt_iとする
        for(i=0;i<m;i++){
            tmp = "ds_"+to_string(i);
            VarName[i] = tmp;
        }
        for(i=m;i<2*m;i++){
            tmp = "de_"+to_string(i);
            VarName[i] = tmp;
        }
        DepotTime = model.addVars(lblist,ublist,objlist,typelist,VarName,2*m);
        delete[] lblist;
        delete[] ublist;
        delete[] objlist;
        delete[] typelist;
        delete[] VarName;

        //乗車時間
        lblist = new double[n+1];
        for(int i=0;i<=n;i++) lblist[i] = 0.0;
        ublist = new double[n+1];
        for(int i=0;i<=n;i++) ublist[i]=inputdata.getMaximumRouteDuration(); 
        objlist = new double[n+1];//係数(とりあえず1にした)
        for(int i=0;i<=n;i++) objlist[i] = 1.0;
        typelist = new char[n+1];
        for(int i=0;i<=n;i++) typelist[i] = GRB_CONTINUOUS;
        VarName = new string[n+1];//名前 ノードiの出発時刻をt_iとする
        VarName[0] = "dummy";
        for(i=1;i<=m;i++){
            tmp = "rt"+to_string(i);
            VarName[i] = tmp;
        }
        RideTime = model.addVars(lblist,ublist,objlist,typelist,VarName,2*m);
        delete[] lblist;
        delete[] ublist;
        delete[] objlist;
        delete[] typelist;
        delete[] VarName;
        GRBLinExpr siki;
        siki = DepartureTime[1] + DepartureTime[2] + DepartureTime[3] + DepartureTime[4] + DepartureTime[5] + DepartureTime[6];
        // model.setObjective(DepartureTime[1] + DepartureTime[2] + t3 + DepartureTime[4] + DepartureTime[5] + DepartureTime[6] + rt1 + rt2 + rt3, GRB_MINIMIZE);
        model.setObjective(siki, GRB_MINIMIZE);
        // model.optimize();

    } catch (GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch (...) {
        cout << "Error during optimization" << endl;
    }
    
    Solution solution(n,m); //初期化 もっと前に初期化してもいいかも
    // for(i=0;i<10000;i++){
    //     RouteList.Change()
    //      distance = Cost.Calc(RouteList);
    //     penalty = solution.calc(RouteList);
    //     if (distance + penalty is best){
    //         bestsolution = solution;
                // bestRouteList = RouteList
    //     }
    // }
    
    solution.test();
    // solution.CalcRouteDistance(&RouteList);
  
}
