#include <iostream>
#include <cmath>
#include <tuple>
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
    int worstPosition;
    double tmpPenalty; 
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

    // ルートの総距離と時間枠ペナルティと乗客数のペナルティ比
    double ALPHA = 1.0; //ルートの距離
    double BETA = 5.0; //時間枠ペナ
    double GAMMA = 1.0; //乗客数ペナ

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

        TotalPenalty = ALPHA*RouteDistance + BETA*model.get(GRB_DoubleAttr_ObjVal);
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

        // worstPosition = 1; 
        // tmpPenalty = 0;
        // for(i=1;i<=2*n;i++){
        //     if (DepartureTimePenalty[i].get(GRB_DoubleAttr_X) > tmpPenalty){
        //         worstPosition = i;
        //         tmpPenalty = DepartureTimePenalty[i].get(GRB_DoubleAttr_X);
        //     }
        // }
        cout << "worstのノード "  << worstPosition << endl;
        for(i=0;i<routelist.getRouteListSize();i++){
            for(j=0;j<routelist.getRouteSize(i);j++){
                cout << routelist.getRoute(i,j) << " ";
            }
            cout << endl;
        }


        // **************************イテレーション開始************************************
        RouteList bestroutelist(m);
        bool recent_changed_flag[m];
        for (int tmp=0;tmp<m;tmp++) recent_changed_flag[tmp] = true;
        tuple<int, int> TmpTuple;
        double QP;
        int NumberOfImprove;
        int search_count = 0;
        int COUNT_MAX =5000;
        double PenaltyArray[m];
        while(search_count < COUNT_MAX){ //一定回数に達したら終了
            // ルートの数だけ、改善がなくなるまで局所探索
            NumberOfImprove=0;
            for (int RouteIndex=0;RouteIndex<m;RouteIndex++){ //車両ごと
                if (!recent_changed_flag[RouteIndex]) continue;
                cout << RouteIndex << "スタート" << endl;
                cout << "RouteIndex:" <<RouteIndex << " " << pow((routelist.getRouteSize(RouteIndex)-2),2)*2 << endl;
                for (int neighborhood=0;neighborhood<pow((routelist.getRouteSize(RouteIndex)-2),2)*2;neighborhood++){ //近傍サイズを探索
                    search_count++;
                    RouteList *TmpRouteList;
                    TmpRouteList = new RouteList(m); //メモリの確保
                    GRBTempConstr *tempconstr;
                    tempconstr = new GRBTempConstr;
                    *TmpRouteList = routelist;
                    // TmpRouteList->InnerOrderChange_requestset(RouteIndex); //リクエストをセットでルート内
                    TmpRouteList->InnerOrderChange_node(n,RouteIndex); //ノードごとにルート内

                    // ルートの制約を追加
                    // ここでdistanceは計算しちゃう
                    QP=0;
                    RouteDistance=0;
                    for(i=0;i<TmpRouteList->getRouteListSize();i++){
                        int current_person=0;
                        for(j=1;j<TmpRouteList->getRouteSize(i)-2;j++){
                            if (TmpRouteList->getRoute(i,j)<=n){
                                current_person+=1;
                            }else{
                                current_person -= 1;
                            }
                            if (current_person > 6){
                                QP += current_person-6;
                            }
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
                        if (ALPHA*RouteDistance + BETA*model.get(GRB_DoubleAttr_ObjVal) + GAMMA*QP < TotalPenalty){
                            routelist = *TmpRouteList;
                            NumberOfImprove += 1;
                            if(QP==0) {
                                bestroutelist = *TmpRouteList;
                                TotalPenalty = ALPHA*RouteDistance + BETA*model.get(GRB_DoubleAttr_ObjVal);
                                cout << "改善 " << TotalPenalty << " count:" << search_count  <<endl;
                                BestTotalPenalty = TotalPenalty;
                                BestRouteDistance = RouteDistance;
                                BestPenalty = model.get(GRB_DoubleAttr_ObjVal);
                                for(int TmpRouteNum=0;TmpRouteNum<m;TmpRouteNum++){
                                    double tmpPenalty = 0;
                                    for(int order=1; order<routelist.getRouteSize(TmpRouteNum)-1;order++){
                                        tmpPenalty += DepartureTimePenalty[routelist.getRoute(TmpRouteNum,order)].get(GRB_DoubleAttr_X);
                                        if (routelist.getRoute(TmpRouteNum,order)<=n) tmpPenalty+= RideTimePenalty[routelist.getRoute(TmpRouteNum,order)].get(GRB_DoubleAttr_X);
                                    }
                                    PenaltyArray[TmpRouteNum] = tmpPenalty;
                                }
                                worstPosition = 0; 
                                tmpPenalty = 0;
                                for(i=1;i<=2*n;i++){
                                    if (DepartureTimePenalty[i].get(GRB_DoubleAttr_X) > tmpPenalty){
                                        worstPosition = i;
                                        tmpPenalty = DepartureTimePenalty[i].get(GRB_DoubleAttr_X);
                                    }
                                }
                            }
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
                    if (search_count == COUNT_MAX){
                        break;
                    }
                }
                cout << RouteIndex << "番目のペナルティ:" << PenaltyArray[RouteIndex] << endl;
                for(int jkl=0;jkl<m;jkl++){
                    cout << PenaltyArray[jkl] << " ";
                }
                cout << endl;
                cout << "----------------------------------" << endl;
                if (search_count>=COUNT_MAX) break;
            }
            int maxPenaltyIndex=0;
            int minPenaltyIndex=0;
            double TmpPenalty=-1;
            for(int PA_i=0;PA_i<m;PA_i++){
                cout << PenaltyArray[PA_i] << " ";
                if (PenaltyArray[PA_i]>=TmpPenalty) {
                    TmpPenalty = PenaltyArray[PA_i];
                    maxPenaltyIndex = PA_i;
                }
            }
            cout << endl;
            TmpPenalty = 100000000.0;
            for(int PA_i=0;PA_i<m;PA_i++){
                if (PenaltyArray[PA_i]<=TmpPenalty){
                    TmpPenalty= PenaltyArray[PA_i];
                    minPenaltyIndex=PA_i;
                }
            }
            if (minPenaltyIndex==maxPenaltyIndex){
                minPenaltyIndex = rand()%m;
                maxPenaltyIndex=rand()%m;
                while (minPenaltyIndex==maxPenaltyIndex){
                    maxPenaltyIndex=rand()%m;
                }
            }
            cout << "maxIndex: " << maxPenaltyIndex << " minIndex: " << minPenaltyIndex << endl;
            if (search_count>=COUNT_MAX) break;

            // 改善がない場合は一つ前のルート間近傍をなしにする
            cout << "改善回数" << NumberOfImprove << endl;
            if(NumberOfImprove==0){
                routelist = bestroutelist;
            }
            cout << "worstのノード: " << worstPosition << endl;
            // ルート間の挿入
            // TmpTuple = routelist.OuterRouteChange_random(n); //ランダムにルート間
            // routelist.OuterRouteChange_specified(n,maxPenaltyIndex); //ペナルティの大きいルートのリクエストを交換
            // TmpTuple = routelist.OuterRouteChange_specified_double(n,maxPenaltyIndex,minPenaltyIndex);
            if (worstPosition==0){
                TmpTuple = routelist.OuterRouteChange_random(n); //ランダムにルート間
            }else{
                TmpTuple = routelist.OuterRouteChange_specified_double(n,maxPenaltyIndex,minPenaltyIndex);
            }
            for (int tmp=0;tmp<m;tmp++) recent_changed_flag[tmp] = false;
            recent_changed_flag[get<0>(TmpTuple)] = true;
            recent_changed_flag[get<1>(TmpTuple)] = true;
            routelist.OuterRouteChange_worstNode(n,worstPosition);
            

            for(i=0;i<routelist.getRouteListSize();i++){
                for(j=0;j<routelist.getRouteSize(i);j++){
                    cout << routelist.getRoute(i,j) << " ";
                }
                cout << endl;
            }
        }
        cout << "総カウント数:" << search_count << endl;

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
        for(i=0;i<bestroutelist.getRouteListSize();i++){
            for(j=0;j<bestroutelist.getRouteSize(i);j++){
                cout << bestroutelist.getRoute(i,j) << " ";
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

