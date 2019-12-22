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

        // 乗車時間の区分線形関数を取得 区分数は3つ
        double ridetimeX[n+1][3];
        double ridetimeY[n+1][3];
        for(i=1;i<=n;i++){
            ridetimeX[i][0] = 0;
            ridetimeX[i][1] = cost.getCost(i,i+n);
            ridetimeX[i][2] = cost.getCost(i,i+n)+1;
            ridetimeY[i][0] = 0;
            ridetimeY[i][1] = 0;
            ridetimeY[i][2] = 5;
        }

        // 乗車時間の区分線形関数を追加  区分数は3つ
        for(i=1;i<=n;i++){
            model.addGenConstrPWL(RideTime[i],RideTimePenalty[i],3,ridetimeX[i],ridetimeY[i],"rtpwl");
        }

        // ここまでの設定は最適化の中で不変
        // ここからはイテレーションごとに変化する設定

        // ここでルートを受け取って、ルートの順番の制約を追加する
        // ルートの長さもここで計算する
        double RouteDistance=0; //RouteDistanceはイテレーション毎に0で初期化
        vector<vector<GRBConstr> > RouteOrderConstr;
        vector<GRBConstr> tempvec;
        GRBTempConstr tempconstr;
        string constrname; //制約の名前付け これは重要じゃない
        double tmp_double;
        for(i=0;i<RouteList.getRouteListSize();i++){
            RouteOrderConstr.push_back(tempvec);
            for(j=1;j<RouteList.getRouteSize(i)-2;j++){ 
                // 1から始めるのは最初のデポは別で設定するから 
                // getRouteSize(i)-2なのはj→j+1を考えてるから
                RouteDistance += cost.getCost(RouteList.getRoute(i,j),RouteList.getRoute(i,j+1));
                // 論文の8の式
                tempconstr = DepartureTime[RouteList.getRoute(i,j)] + 10.0 +  cost.getCost(RouteList.getRoute(i,j),RouteList.getRoute(i,j+1)) <= DepartureTime[RouteList.getRoute(i,j+1)];
                constrname = "constr"+to_string(i)+ "_" + to_string(j);
                RouteOrderConstr[i].push_back(model.addConstr(tempconstr,constrname));
            }
        }

        // RouteOrderConstrのサイズは、車両数+1 最後の1つはデポ関連

        // デポの時刻DepotTimeとの制約も追加
        RouteOrderConstr.push_back(tempvec);
            for(i=0;i<RouteList.getRouteListSize();i++){
                RouteDistance += cost.getCost(0,RouteList.getRoute(i,1)); //i番目の車両の1番目
                RouteDistance += cost.getCost(RouteList.getRoute(i,RouteList.getRouteSize(i)-2),0);//i番目の車両のデポを除く最後
                // デポと1番目の制約
                tempconstr = DepotTime[i] + 10.0 +  cost.getCost(0,RouteList.getRoute(i,1)) <= DepartureTime[RouteList.getRoute(i,1)];
                constrname = to_string(i) + "constr_depot_1";
                RouteOrderConstr[RouteList.getRouteListSize()].push_back(model.addConstr(tempconstr,constrname));
                // 最後とデポの制約
                tempconstr = DepartureTime[RouteList.getRoute(i,RouteList.getRouteSize(i)-2)] + 10.0 +  cost.getCost(RouteList.getRoute(i,RouteList.getRouteSize(i)-2),0) <= DepotTime[i+m];
                constrname = to_string(i) + "constr_last_depot";
                RouteOrderConstr[RouteList.getRouteListSize()].push_back(model.addConstr(tempconstr,constrname));
            }
        cout << "RouteDistance:" <<RouteDistance << endl;

        
        // optimize
        model.set(GRB_IntParam_OutputFlag, 0); //ログの出力をoff
        model.optimize();


        // ここでルートの順番の制約をremoveしたい
        for(i=0;i<RouteOrderConstr.size();i++){
            for(j=0;j<RouteOrderConstr[i].size();j++){
                model.remove(RouteOrderConstr[i][j]);
            }
        }
        //RouteOrderConstrのメモリ解放
        vector<vector<GRBConstr> >().swap(RouteOrderConstr);


        // for(i=1;i<=2*n;i++){
        //      cout << DepartureTime[i].get(GRB_StringAttr_VarName) << " "
        //     << DepartureTime[i].get(GRB_DoubleAttr_X) << " "
        //     << DepartureTimePenalty[i].get(GRB_DoubleAttr_X) << endl;

        // }
        // for(i=1;i<=n;i++){
        //      cout << RideTime[i].get(GRB_StringAttr_VarName) << " "
        //     << RideTime[i].get(GRB_DoubleAttr_X) << endl;
        // }

        cout << "penalty: " << model.get(GRB_DoubleAttr_ObjVal) << endl;

        // for(i=0;i<10000;i++){
        //     RouteList.Change();
        //     ルートの制約を追加
        //     penaltyとdistanceを計算
        //     if (distance + penalty is best){
        //         bestroutelist = routelist;
        //     }
        // }


        
    } catch (GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch (...) {
        cout << "Error during optimization" << endl;
    }
  
}
