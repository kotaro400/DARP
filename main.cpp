#include <iostream>
#include <cmath>
#include "InputData.hpp"
#include "RouteList.hpp"
#include "Location.hpp"
#include "Cost.hpp"
#include "myfunction.hpp"
#include <gurobi_c++.h>
#include "cpu_time.c"
using namespace std;



int main(int argc, char *argv[]){
    int i,j;
    string tmp;
    int worstPosition,tmpPenalty; //
    // 入力を受け取り
    InputData inputdata;
    string inputfile = argv[1];
    inputdata.setInputData(inputfile = argv[1]);
    cout << "車両数:" << inputdata.getVehicleNum() << endl;
    cout << "リクエスト数:" << inputdata.getRequestSize() << endl;
    cout << "ルートの最大長さ:" << inputdata.getMaximumRouteDuration() << endl;
    cout << "車両の容量:" << inputdata.getVehicleCapacity() << endl;

    // n:カスタマーサイズ 
    // m:車両数
    int n=inputdata.getRequestSize()/2;
    int m=inputdata.getVehicleNum();

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
    // これおそらくいらない
    for(int i=1;i<=n;i++){
        vector<double> vec{ 0.0, 0.0, cost.getCost(i,i+n),0,cost.getCost(i,i+n)+1.0,1};
        inputdata.setRideTimePenalty(i,vec);
        vec.clear();
    }

    // RouteListクラス:複数のルートをまとめて保持するクラス 
    RouteList routelist(m);
    routelist.makeInitialRoute(inputdata.getRequestSize());

    // ペナルティ
    double TotalPenalty=0;

    // 最適解
    double BestTotalPenalty;
    double BestRouteDistance;
    double BestPenalty;
    
    // クラスをまたいでenvとmodelを保持できなさそうだからmainの中で宣言する
    try{
        GRBEnv env=GRBEnv(true);
        env.set("LogFile", "mip1.log");
        env.start();
        GRBModel model = GRBModel(env);
        GRBVar DepartureTime[2*n+1];
        GRBVar DepotTime[2*m];
        GRBVar RideTime[n+1];
        GRBVar DepartureTimePenalty[2*n+1];
        GRBVar RideTimePenalty[n+1];

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
            model.addConstr(RideTime[i] == DepartureTime[i+n]-DepartureTime[i]-10,tmp);
        }

        // 乗車時間の区分線形関数を取得 区分数は3つ
        double ridetimeX[n+1][3];
        double ridetimeY[n+1][3];
        for(i=1;i<=n;i++){
            ridetimeX[i][0] = 0;
            // ridetimeX[i][1] = cost.getCost(i,i+n);
            // ridetimeX[i][2] = cost.getCost(i,i+n)+1;
            ridetimeX[i][1] = 90;
            ridetimeX[i][2] = 91;
            ridetimeY[i][0] = 0;
            ridetimeY[i][1] = 0;
            ridetimeY[i][2] = 1;
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
        vector<GRBConstr> RouteOrderConstr;
        GRBTempConstr *tempconstr;
        tempconstr = new GRBTempConstr;
        string constrname; //制約の名前付け これは重要じゃない
        double tmp_double;
        for(i=0;i<routelist.getRouteListSize();i++){
            for(j=1;j<routelist.getRouteSize(i)-2;j++){ 
                // 1から始めるのは最初のデポは別で設定するから 
                // getRouteSize(i)-2なのはj→j+1を考えてるから
                RouteDistance += cost.getCost(routelist.getRoute(i,j),routelist.getRoute(i,j+1));
                // 論文の8の式
                *tempconstr = DepartureTime[routelist.getRoute(i,j)] + 10.0 +  cost.getCost(routelist.getRoute(i,j),routelist.getRoute(i,j+1)) <= DepartureTime[routelist.getRoute(i,j+1)];
                constrname = "constr"+to_string(i)+ "_" + to_string(j);
                RouteOrderConstr.push_back(model.addConstr(*tempconstr,constrname));
            }
        }
        // デポの時刻DepotTimeとの制約も追加
        for(i=0;i<routelist.getRouteListSize();i++){
            RouteDistance += cost.getCost(0,routelist.getRoute(i,1)); //i番目の車両の1番目
            RouteDistance += cost.getCost(routelist.getRoute(i,routelist.getRouteSize(i)-2),0);//i番目の車両のデポを除く最後
            // デポと1番目の制約
            *tempconstr = DepotTime[i] +  cost.getCost(0,routelist.getRoute(i,1)) <= DepartureTime[routelist.getRoute(i,1)];
            constrname = to_string(i) + "constr_depot_1";
            RouteOrderConstr.push_back(model.addConstr(*tempconstr,constrname));
            // 最後とデポの制約
            *tempconstr = DepartureTime[routelist.getRoute(i,routelist.getRouteSize(i)-2)] + 10.0 +  cost.getCost(routelist.getRoute(i,routelist.getRouteSize(i)-2),0) == DepotTime[i+m];
            constrname = to_string(i) + "constr_last_depot";
            RouteOrderConstr.push_back(model.addConstr(*tempconstr,constrname));
        }

        // MaximumRouteDurationを設定
        for(i=0;i<m;i++){
            tmp = "MaximumRouteDutaion"+to_string(i);
            model.addConstr(480 >= DepotTime[i+m]-DepotTime[i],tmp);
        }

        // 計算開始時間
        double starttime = cpu_time();
        // optimize
        model.set(GRB_IntParam_OutputFlag, 0); //ログの出力をoff
        model.optimize();

        cout << "RouteDistance:" <<RouteDistance << endl;
        cout << "penalty: " << model.get(GRB_DoubleAttr_ObjVal) << endl;

        TotalPenalty = RouteDistance + model.get(GRB_DoubleAttr_ObjVal);
        BestTotalPenalty = TotalPenalty;
        BestRouteDistance = RouteDistance;
        BestPenalty = model.get(GRB_DoubleAttr_ObjVal);
        // ここでルートの順番の制約をremove
        for(i=0;i<RouteOrderConstr.size();i++){
            model.remove(RouteOrderConstr[i]);
        }
        //RouteOrderConstrとtempconstrのメモリ解放
        delete tempconstr;
        vector<GRBConstr>().swap(RouteOrderConstr);

        worstPosition = 1; 
        tmpPenalty = 0;
        for(i=1;i<=2*n;i++){
            if (DepartureTimePenalty[i].get(GRB_DoubleAttr_X) > tmpPenalty){
                worstPosition = i;
                tmpPenalty = DepartureTimePenalty[i].get(GRB_DoubleAttr_X);
            }
        }
        cout << "worstのノード "  << worstPosition << endl;
        for(i=0;i<routelist.getRouteListSize();i++){
            for(j=0;j<routelist.getRouteSize(i);j++){
                cout << routelist.getRoute(i,j) << " ";
            }
            cout << endl;
        }

        // routelist.InnerRouteChange_specified(n,worstPosition);


        // **************************イテレーション開始************************************

        // ルートの数だけ、改善がなくなるまで局所探索
        double PenaltyArray[routelist.getRouteListSize()];
        for (int RouteIndex=0;RouteIndex<routelist.getRouteListSize();RouteIndex++){
            cout << RouteIndex <<" " << pow((routelist.getRouteSize(RouteIndex)-2)/2,2) <<  endl;
            for (int neighborhood=0;neighborhood<pow((routelist.getRouteSize(RouteIndex)-2)/2,2)*4;neighborhood++){
                RouteList *TmpRouteList;
                TmpRouteList = new RouteList(m); //メモリの確保
                GRBTempConstr *tempconstr;
                tempconstr = new GRBTempConstr;
                *TmpRouteList = routelist;
                TmpRouteList->InnerOrderChange_requestset(RouteIndex);

                // ルートの制約を追加
                // ここでdistanceは計算しちゃう
                RouteDistance=0;
                for(i=0;i<TmpRouteList->getRouteListSize();i++){
                    for(j=1;j<TmpRouteList->getRouteSize(i)-2;j++){
                        RouteDistance += cost.getCost(TmpRouteList->getRoute(i,j),TmpRouteList->getRoute(i,j+1));
                        // 論文8の式
                        *tempconstr = DepartureTime[TmpRouteList->getRoute(i,j)] + 10.0 + cost.getCost(TmpRouteList->getRoute(i,j),TmpRouteList->getRoute(i,j+1)) <= DepartureTime[TmpRouteList->getRoute(i,j+1)];
                        constrname = "constr"+to_string(i)+ "_" + to_string(j);
                        RouteOrderConstr.push_back(model.addConstr(*tempconstr,constrname));
                    }
                }
                
                // デポの時刻DepotTimeとの制約も追加
                for(i=0;i<TmpRouteList->getRouteListSize();i++){
                    RouteDistance += cost.getCost(0,TmpRouteList->getRoute(i,1));
                    RouteDistance += cost.getCost(TmpRouteList->getRouteSize(i)-2,0);
                    // デポと1番目の制約
                    *tempconstr = DepotTime[i] + cost.getCost(0,TmpRouteList->getRoute(i,1)) <= DepartureTime[TmpRouteList->getRoute(i,1)];
                    constrname = to_string(i) + "constr_depot_1";
                    RouteOrderConstr.push_back(model.addConstr(*tempconstr,constrname));
                    // 最後とデポの制約
                    *tempconstr = DepartureTime[TmpRouteList->getRoute(i,TmpRouteList->getRouteSize(i)-2)] + 10.0 + cost.getCost(TmpRouteList->getRoute(i,TmpRouteList->getRouteSize(i)-2),0) == DepotTime[i+m];
                    constrname = to_string(i) + "constr_last_depot";
                    RouteOrderConstr.push_back(model.addConstr(*tempconstr,constrname));
                    
                    // LP実行(optimize)
                    model.optimize();
                    // ペナルティを計算して比較
                    // 良い解の場合 
                    if (RouteDistance + model.get(GRB_DoubleAttr_ObjVal) < TotalPenalty){
                        routelist = *TmpRouteList;
                        TotalPenalty = RouteDistance + model.get(GRB_DoubleAttr_ObjVal);
                        cout << "改善 " << TotalPenalty << endl;
                        BestTotalPenalty = TotalPenalty;
                        BestRouteDistance = RouteDistance;
                        BestPenalty = model.get(GRB_DoubleAttr_ObjVal);
                        double tmpPenalty = 0;
                        for(int order=0; order<routelist.getRouteSize(RouteIndex);order++){
                            tmpPenalty += DepartureTimePenalty[routelist.getRoute(RouteIndex,order)].get(GRB_DoubleAttr_X);
                        }
                        PenaltyArray[RouteIndex] = tmpPenalty;
                    }
                    // 悪い解ならなにもしない
                    // ルートの制約をremove
                    for(i=0;i<RouteOrderConstr.size();i++){
                        model.remove(RouteOrderConstr[i]);
                    }

                    //RouteOrderConstrとtemoconstrのメモリ解放
                    vector<GRBConstr>().swap(RouteOrderConstr);
                    delete tempconstr;

                    //TmpRouteListクラスのメモリ解放
                    delete TmpRouteList; 
                }
            }
            cout << RouteIndex << "番目のペナルティ:" << PenaltyArray[RouteIndex] << endl;
            cout << "----------------------------------" << endl;
        }

        // for(int k=1;k<10;k++){
        //     RouteList *TmpRouteList;
        //     TmpRouteList = new RouteList(m); //メモリの確保
        //     GRBTempConstr *tempconstr;
        //     tempconstr = new GRBTempConstr;
        //     if (k%100 == 0){ 
        //         cout << "100で割り切れる" << endl;
        //         // routelist.OuterRouteChange_random(n);
        //         // cout << routelist.getRouteSize(0) << ","<< routelist.getRouteSize(1) << ","<< routelist.getRouteSize(2) << endl;
        //     }
        //     *TmpRouteList = routelist;
        //     // TmpRouteList->InnerRouteChange_node(n); //近傍解 ノードごとの挿入近傍 ランダム
        //     // TmpRouteList->InnerRouteChange_requestSet(); //近傍解 リクエストごとの挿入近傍 ランダム
        //     // TmpRouteList->InnerRouteChange_specified(n,worstPosition); //近傍解 ペナルティの大きいノードを挿入
        //     TmpRouteList->InnerOrderChange_requestset(0);

        //     // for(i=0;i<TmpRouteList->getRouteListSize();i++){
        //     //     for(j=0;j<TmpRouteList->getRouteSize(i);j++){
        //     //         cout << TmpRouteList->getRoute(i,j) << " ";
        //     //     }
        //     //     cout << endl;
        //     // }

        //     // ルートの制約を追加
        //     // ここでdistanceは計算しちゃう
        //     RouteDistance=0;
        //     for(i=0;i<TmpRouteList->getRouteListSize();i++){
        //         for(j=1;j<TmpRouteList->getRouteSize(i)-2;j++){
        //             RouteDistance += cost.getCost(TmpRouteList->getRoute(i,j),TmpRouteList->getRoute(i,j+1));
        //             // 論文8の式
        //             *tempconstr = DepartureTime[TmpRouteList->getRoute(i,j)] + 10.0 + cost.getCost(TmpRouteList->getRoute(i,j),TmpRouteList->getRoute(i,j+1)) <= DepartureTime[TmpRouteList->getRoute(i,j+1)];
        //             constrname = "constr"+to_string(i)+ "_" + to_string(j);
        //             RouteOrderConstr.push_back(model.addConstr(*tempconstr,constrname));
        //         }
        //     }

        //     // TODO デポの時刻DepotTimeとの制約も追加
        //     for(i=0;i<TmpRouteList->getRouteListSize();i++){
        //         RouteDistance += cost.getCost(0,TmpRouteList->getRoute(i,1));
        //         RouteDistance += cost.getCost(TmpRouteList->getRouteSize(i)-2,0);
        //         // デポと1番目の制約
        //         *tempconstr = DepotTime[i] + cost.getCost(0,TmpRouteList->getRoute(i,1)) <= DepartureTime[TmpRouteList->getRoute(i,1)];
        //         constrname = to_string(i) + "constr_depot_1";
        //         RouteOrderConstr.push_back(model.addConstr(*tempconstr,constrname));
        //         // 最後とデポの制約
        //         *tempconstr = DepartureTime[TmpRouteList->getRoute(i,TmpRouteList->getRouteSize(i)-2)] + 10.0 + cost.getCost(TmpRouteList->getRoute(i,TmpRouteList->getRouteSize(i)-2),0) == DepotTime[i+m];
        //         constrname = to_string(i) + "constr_last_depot";
        //         RouteOrderConstr.push_back(model.addConstr(*tempconstr,constrname));

        //     }

        //     // LP実行(optimize)
        //     model.optimize();
        //     // ペナルティを計算して比較
        //     // 良い解の場合
        //     if (RouteDistance + model.get(GRB_DoubleAttr_ObjVal) < TotalPenalty){
        //         routelist = *TmpRouteList;
        //         TotalPenalty = RouteDistance + model.get(GRB_DoubleAttr_ObjVal);
        //         cout << "改善 " << TotalPenalty << endl;
        //         BestTotalPenalty = TotalPenalty;
        //         BestRouteDistance = RouteDistance;
        //         BestPenalty = model.get(GRB_DoubleAttr_ObjVal);
        //     }
        //     // 悪い解ならなにもしない

        //     // ルートの中で一番ペナルティが大きいノードを見つける
        //     worstPosition = 1; 
        //     tmpPenalty = 0;
        //     for(i=1;i<=2*n;i++){
        //         if (DepartureTimePenalty[i].get(GRB_DoubleAttr_X) > tmpPenalty){
        //             worstPosition = i;
        //             tmpPenalty = DepartureTimePenalty[i].get(GRB_DoubleAttr_X);
        //         }
        //     }
        //     // cout << "worstのノード "  << worstPosition << endl;

        //     // ルートの制約をremove
        //     for(i=0;i<RouteOrderConstr.size();i++){
        //         model.remove(RouteOrderConstr[i]);
        //     }
        //     //RouteOrderConstrとtemoconstrのメモリ解放
        //     vector<GRBConstr>().swap(RouteOrderConstr);
        //     delete tempconstr;

        //     //TmpRouteListクラスのメモリ解放
        //     delete TmpRouteList; 
        // }
        cout << "RouteDistance: " << BestRouteDistance << endl;
        cout << "bestpena:"<<BestPenalty << endl;

        // for(i=1;i<=2*n;i++){
        //      cout << DepartureTime[i].get(GRB_StringAttr_VarName) << " "
        //     << DepartureTime[i].get(GRB_DoubleAttr_X) << " "
        //     << DepartureTimePenalty[i].get(GRB_DoubleAttr_X) << endl;
        // }
        // for(i=1;i<=n;i++){
        //     cout << RideTime[i].get(GRB_StringAttr_VarName) << " "
        //     << RideTime[i].get(GRB_DoubleAttr_X) << " "
        //     << RideTimePenalty[i].get(GRB_DoubleAttr_X) << endl;
        // }
        
        // 計算終了の時間
        double endtime = cpu_time();

        cout << "時間:" << endtime -starttime << endl;
        // for(i=0;i<m;i++){
        //     cout << i << " " << DepotTime[i].get(GRB_DoubleAttr_X) << " "
        //     << DepotTime[i+m].get(GRB_DoubleAttr_X) << " " << 
        //     DepotTime[i+m].get(GRB_DoubleAttr_X)-DepotTime[i].get(GRB_DoubleAttr_X) << endl;
        // }
        for(i=0;i<routelist.getRouteListSize();i++){
            for(j=0;j<routelist.getRouteSize(i);j++){
                cout << routelist.getRoute(i,j) << " ";
            }
            cout << endl;
        }

        
    } catch (GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch (...) {
        cout << "Error during optimization" << endl;
    }
  
}

