#include <iostream>
#include "InputData.hpp"
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
        vector<double> vec{ 0.0, 0.0, cost.getCost(i,i+inputdata.getRequestSize()/2),0,cost.getCost(i,i+inputdata.getRequestSize()/2)+1.0,5};
        inputdata.setRideTimePenalty(i,vec);
        vec.clear();
    }

    // RouteListクラス:複数のルートをまとめて保持するクラス 
    RouteList RouteList(inputdata.getVehicleNum());
    RouteList.makeInitialRoute(inputdata.getRequestSize());
    
// クラスでenvとmodelを保持できなさそうだからmainの中で宣言する

    // n:カスタマーサイズ 
    // m:車両数
    int n=inputdata.getRequestSize()/2;
    int m=inputdata.getVehicleNum();
    try{
        GRBEnv env=GRBEnv(true);
        env.set("LogFile", "mip1.log");
        env.start();
        GRBModel model = GRBModel(env);
        GRBVar DepartureTime[2*n+1];
        GRBVar DepotTime[2*m];
        GRBVar RideTime[n+1];
        GRBVar DepartureTimePenalty[2*n+1];
        GRBVar DepotTimePenalty[2*m];
        GRBVar RideTimePenalty[n+1];

        string tmp;
        // 各ノードの出発時刻の変数を追加
        for(i=0;i<=2*n;i++){
            tmp = "t_"+to_string(i); //ノードiの出発時刻をt_iとする
            DepartureTime[i] = model.addVar(0.0,(double)inputdata.getLatestArrivalDepotTime(),0.0,GRB_CONTINUOUS,tmp);
        }
        for(i=0;i<=2*n;i++){
            tmp = "p_t_"+to_string(i); //ノードiの出発時刻をt_iとする
            DepartureTimePenalty[i] = model.addVar(0.0,(double)inputdata.getLatestArrivalDepotTime(),0.0,GRB_CONTINUOUS,tmp);
        }

        // デポの出発と帰る時刻
        for(i=0;i<m;i++){
            tmp = "ds_"+to_string(i);
            DepotTime[i] = model.addVar(0.0,(double)inputdata.getLatestArrivalDepotTime(),0.0,GRB_CONTINUOUS,tmp);
        }
        for(i=m;i<2*m;i++){
            tmp = "de_"+to_string(i);
            DepotTime[i] = model.addVar(0.0,(double)inputdata.getLatestArrivalDepotTime(),0.0,GRB_CONTINUOUS,tmp);
        }
        for(i=0;i<m;i++){
            tmp = "p_ds_"+to_string(i);
            DepotTimePenalty[i] = model.addVar(0.0,(double)inputdata.getLatestArrivalDepotTime(),0.0,GRB_CONTINUOUS,tmp);
        }
        for(i=m;i<2*m;i++){
            tmp = "p_de_"+to_string(i);
            DepotTimePenalty[i] = model.addVar(0.0,(double)inputdata.getLatestArrivalDepotTime(),0.0,GRB_CONTINUOUS,tmp);
        }

        //乗車時間
        for (i=0;i<=n;i++){
            tmp = "rt"+to_string(i);
            RideTime[i] = model.addVar(0.0,(double)inputdata.getLatestArrivalDepotTime(),0.0,GRB_CONTINUOUS,tmp);
        }
        for (i=0;i<=n;i++){
            tmp = "p_rt"+to_string(i);
            RideTimePenalty[i] = model.addVar(0.0,(double)inputdata.getLatestArrivalDepotTime(),0.0,GRB_CONTINUOUS,tmp);
        }
        // ここまで変数とペナルティ変数を定義

        // 出発時刻のペナルティを取得 区分数は4
        double departureX[2*n+1][4];
        double departureY[2*n+1][4];
        for (i=1;i<=n;i++){ //pickup
            for(j=0;j<4;j++){
                departureX[i][j] = inputdata.getPickupPointer(i)->getPickupPenaltyXValue(j);
                departureY[i][j] =  inputdata.getPickupPointer(i)->getPickupPenaltyYValue(j);
            }
        }
        for(i=n+1;i<=2*n;i++){ //dropoff
            for(j=0;j<4;j++){
                departureX[i][j] = inputdata.getDropoffPointer(i-n)->getDropoffPenaltyXValue(j);
                departureY[i][j] = inputdata.getDropoffPointer(i-n)->getDropoffPenaltyYValue(j);
            }
        }
        // 出発時刻に関する区分線形関数を追加
        for(i=1;i<=2*n;i++){
            model.addGenConstrPWL(DepartureTime[i],DepartureTimePenalty[i],4,departureX[i],departureY[i],"pwl");
        }
        
        // 目的関数を追加
        GRBLinExpr objection;
        for(i=1;i<=2*n;i++){
            objection += DepartureTimePenalty[i];
        }
        for(i=1;i<=n;i++){
            objection += RideTimePenalty[i];
        }
        model.setObjective(objection, GRB_MINIMIZE);

        
        // 乗車時間の定義 rt_i = t_i+n -t_i （制約として追加)
        for (i=1;i<=n;i++){
            tmp = "RideConst"+to_string(i);
            model.addConstr(RideTime[i] == DepartureTime[i+n]-DepartureTime[i],tmp);
        }

        // 乗車時間の区分線形関数を追加
        // 区分数は3つ
        // xvec.push_back({0.0}); //ダミー index0は使わない
        // yvec.push_back({0.0}); //ダミー
        // for(i=1;i<=n;i++){
        //     xvec.push_back(tempvec);
        //     yvec.push_back(tempvec);
        //     for(j=0;j<inputdata.getDropoffPointer(i)->getRidetimePenaltyX()->size();j++){
        //         xvec[i].push_back(inputdata.getDropoffPointer(i)->getRidetimePenaltyXValue(j));
        //         yvec[i].push_back(inputdata.getDropoffPointer(i)->getRidetimePenaltyYValue(j));
        //     }
        // }
        // ここまででxvecとyvecに乗車時間のペナルティを追加
        // 区分線形関数を追加
        // vectorを配列に変換
        // for(i=1;i<=n;i++){
        //     double *xpointer;
        //     double *ypointer;
        //     xpointer = new double[xvec[i].size()];
        //     ypointer = new double[yvec[i].size()];
        //     for(j=0;j<xvec[i].size();j++){
        //         xpointer[j]=xvec[i][j];
        //         ypointer[j]=yvec[i][j];
        //     }
        //     // model.setPWLObj(RideTime[i],3,xpointer,ypointer);
        //     delete[] xpointer;
        //     delete[] ypointer;
        // }
        // xvec.clear();
        // yvec.clear();
        // tempvec.clear();


        // TODO ここでルートを受け取って、ルートの順番の制約を追加する
        // GRBTempConstr tempconstr;
        // string constrname; //制約の名前付け これは重要じゃない
        // double RouteDistance;
        // double tmp_double; 
        // for(i=0;i<RouteList.getRouteListSize();i++){
        //     for(j=0;j<RouteList.getRouteSize(i)-1;j++){
        //         // cout << RouteList.getRoute(i,j) << " " << RouteList.getRoute(i,j+1) << " ";
        //         tmp_double = cost.getCost(RouteList.getRoute(i,j),RouteList.getRoute(i,j+1));
        //         RouteDistance += tmp_double;
        //         tempconstr = DepartureTime[i] + 10.0 + tmp_double <= DepartureTime[i+1];
        //         constrname = "constr"+to_string(i)+ "_" + to_string(j);
        //         model.addConstr(tempconstr,constrname);
        //     }
        // }
        // cout << RouteDistance << endl;
        // cout << RouteList.getRouteListSize() << endl;


        // デポの時刻DepotTimeとの制約も追加
        

        // optimize
        model.optimize();
        for(i=1;i<=2*n;i++){
             cout << DepartureTime[i].get(GRB_StringAttr_VarName) << " "
            << DepartureTime[i].get(GRB_DoubleAttr_X) << endl;
        }
        for(i=1;i<=n;i++){
             cout << RideTime[i].get(GRB_StringAttr_VarName) << " "
            << RideTime[i].get(GRB_DoubleAttr_X) << endl;
        }
        cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << endl;


        // RouteDistance = cost.CalcDistance(&RouteList); //ルートの総距離

        
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
  
}
