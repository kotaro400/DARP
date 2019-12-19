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
        for(int i=0;i<=2*n;i++) ublist[i]=inputdata.getLatestArrivalDepotTime();
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
        for(int i=0;i<2*m;i++) ublist[i]=inputdata.getLatestArrivalDepotTime();
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
        for(int i=0;i<=n;i++) ublist[i]=inputdata.getLatestArrivalDepotTime(); 
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
        RideTime = model.addVars(lblist,ublist,objlist,typelist,VarName,n+1);
        delete[] lblist;
        delete[] ublist;
        delete[] objlist;
        delete[] typelist;
        delete[] VarName;

        // xvecとyvecはpick,drop,rideで使い回す
        vector<vector<double> > xvec;
        vector<vector<double> > yvec;
        // xvec:区分線形関数のx座標
        // yvec:区分線形関数のy座標
        xvec.push_back({0.0}); //ダミー index0は使わない
        yvec.push_back({0.0}); //ダミー
        // 乗車時間の区分線形関数を設定
        vector<double> tempvec;
        for (i=1;i<=n;i++){
            xvec.push_back(tempvec);
            yvec.push_back(tempvec);
            for (j=0;j<inputdata.getPickupPointer(i)->getPickupPenaltyX()->size();j++){
                xvec[i].push_back(inputdata.getPickupPointer(i)->getPickupPenaltyXValue(j));
                yvec[i].push_back(inputdata.getPickupPointer(i)->getPickupPenaltyYValue(j));
            }
        }
        for(i=n+1;i<=2*n;i++){
            xvec.push_back(tempvec);
            yvec.push_back(tempvec);
            // cout << inputdata.getDropoffPointer(i-n)->getDropoffPenaltyX()->size() << endl; //ここがずっと0になってる
            for(j=0;j<inputdata.getDropoffPointer(i-n)->getDropoffPenaltyX()->size();j++){
                xvec[i].push_back(inputdata.getDropoffPointer(i-n)->getDropoffPenaltyXValue(j));
                yvec[i].push_back(inputdata.getDropoffPointer(i-n)->getDropoffPenaltyYValue(j));
            }
        }
        // ここまででxvecとyvecに乗降のペナルティを追加
        // 区分線形関数を追加

        // vectorを配列に変換
        // gurobiの入力がvectorをうけとらないため
        for(i=1;i<=2*n;i++){
            double *xpointer;
            double *ypointer;
            xpointer = new double[xvec[i].size()];
            ypointer = new double[yvec[i].size()];
            for(j=0;j<xvec[i].size();j++){
                xpointer[j]=xvec[i][j];
                ypointer[j]=yvec[i][j];
            }
            model.setPWLObj(DepartureTime[i],4,xpointer,ypointer);
            delete[] xpointer;
            delete[] ypointer;
        }
        xvec.clear();
        yvec.clear();
        tempvec.clear();



        // 目的関数を追加
        GRBLinExpr objection;
        for(i=1;i<=2*n;i++){
            objection += DepartureTime[i];
        }
        for(i=1;i<=n;i++){
            objection += RideTime[i];
        }
        model.setObjective(objection, GRB_MINIMIZE);

        // 乗車時間の定義 rt_i = t_i+n -t_i （制約として追加)
        for (i=1;i<=n;i++){
            tmp = "RideConst"+to_string(i);
            model.addConstr(RideTime[i] == DepartureTime[i+n]-DepartureTime[i],tmp);
        }

        // 乗車時間の区分線形関数を追加
        // 区分数は3つ
        xvec.push_back({0.0}); //ダミー index0は使わない
        yvec.push_back({0.0}); //ダミー
        for(i=1;i<=n;i++){
            xvec.push_back(tempvec);
            yvec.push_back(tempvec);
            for(j=0;j<inputdata.getDropoffPointer(i)->getRidetimePenaltyX()->size();j++){
                xvec[i].push_back(inputdata.getDropoffPointer(i)->getRidetimePenaltyXValue(j));
                yvec[i].push_back(inputdata.getDropoffPointer(i)->getRidetimePenaltyYValue(j));
            }
        }
        // ここまででxvecとyvecに乗車時間のペナルティを追加

        // 区分線形関数を追加

        // vectorを配列に変換
        for(i=1;i<=n;i++){
            double *xpointer;
            double *ypointer;
            xpointer = new double[xvec[i].size()];
            ypointer = new double[yvec[i].size()];
            for(j=0;j<xvec[i].size();j++){
                xpointer[j]=xvec[i][j];
                ypointer[j]=yvec[i][j];
            }
            model.setPWLObj(RideTime[i],3,xpointer,ypointer);
            delete[] xpointer;
            delete[] ypointer;
        }
        xvec.clear();
        yvec.clear();
        tempvec.clear();


        // TODO ここでルートを受け取って、ルートの順番の制約を追加する
        // デポの時刻DepotTimeとの制約も追加
        
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
    // solution.CalcRouteDistance(&RouteList);
  
}
