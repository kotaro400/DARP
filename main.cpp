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
    try{
        GRBEnv env = GRBEnv();
    } catch (GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch (...) {
        cout << "Error during optimization" << endl;
    }
    
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
    GRBEnv env=GRBEnv(true);
    env.set("LogFile", "mip1.log");
    env.start();
    GRBModel model = GRBModel(env);

    int n=inputdata.getRequestSize()/2;
    int m=inputdata.getVehicleNum();
    GRBVar* DepartureTime = 0;
    GRBVar* DepotTime = 0;
    GRBVar* RideTime = 0;

    // 各ノードの出発時刻の変数を追加
    double lblist[2*n+1] = {0.0}; //下限
    double ublist[2*n+1]; //上限
    for(int i=0;i<=2*n;i++) ublist[i]=inputdata.getMaximumRouteDuration(); 
    double objlist[2*n+1]; //係数(とりあえず1にした)
    for(int i=0;i<=2*n;i++) objlist[i] = 1.0;
    char typelist[2*n+1]; //type
    for(int i=0;i<=2*n;i++) typelist[i] = GRB_CONTINUOUS;
    string DepartureName[2*n+1]; //名前 ノードiの出発時刻をt_iとする
    DepartureName[0] = "depot";
    string tmp;
    for(i=1;i<=2*n;i++){
        tmp = "t_"+to_string(i);
        DepartureName[i] = tmp;
    }
    DepartureTime = model.addVars(lblist,ublist,objlist,typelist,DepartureName,2*n+1);

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
