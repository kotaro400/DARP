#include <iostream>
#include <cmath>
#include <tuple>
#include <utility>
#include <algorithm>
#include "InputData.hpp"
#include "PiecewiseLinear.hpp"
#include "RouteList.hpp"
#include "Location.hpp"
#include "Cost.hpp"
#include "myfunction.hpp"
#include "cpu_time.c"
#include <numeric>
#include <fstream>
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
        inputdata.setRideTimePenalty(i, cost.getCost(i,i+n));
    }

    // RouteListクラス:複数のルートをまとめて保持するクラス
    RouteList routelist(m, n);
    routelist.makeInitialRoute(inputdata.getRequestSize());


    // ルートの総距離と時間枠ペナルティと乗客数のペナルティ比
    double ALPHA = 1.0; //ルートの距離
    double BETA = 1.0; //時間枠ペナ
    double GAMMA = 1.0; //乗客数ペナ

    // イテレーション回数
    // int COUNT_MAX = 100000;

    double BestPenalty;


    for(int i = 0; i < m; i++){
      routelist.ComputeStartTimes(&cost, &inputdata, i);
    }

    // ペナルティの計算
    // BestPenalty = ALPHA * cost.CalcDistance(&routelist);
    for(int i = 0; i < m; i++){
      // BestPenalty += BETA * routelist.min_penalties[i][routelist.getRouteSize(i) - 2].computeValue(routelist.start_times[i][routelist.getRouteSize(i) - 2]);
      for(int j = 1; j < routelist.getRouteSize(i) - 1; j++){
        if(routelist.getRoute(i, j) <= n){
          BestPenalty += BETA * inputdata.getPickupPointer(routelist.getRoute(i, j))->penalty.computeValue(routelist.start_times[i][j]);
          if(inputdata.getPickupPointer(routelist.getRoute(i, j))->maxRideTime < routelist.ride_times[routelist.getRoute(i, j) - 1]){
            BestPenalty += BETA * (routelist.ride_times[routelist.getRoute(i, j) - 1] - inputdata.getPickupPointer(routelist.getRoute(i, j))->maxRideTime);
          }
        }else{
          BestPenalty += BETA * inputdata.getDropoffPointer(routelist.getRoute(i, j) - n)->penalty.computeValue(routelist.start_times[i][j]);
        }
      }
    }

    double starttime = cpu_time();

    for(int routeindex = 0; routeindex < m; routeindex++){
      for(int it = 0; it < pow((routelist.getRouteSize(routeindex)-2),2)*2; it++){
        tmpPenalty = 0;
        RouteList *tmp_routelist;
        tmp_routelist = new RouteList(m, n);
        *tmp_routelist = routelist;

        tmp_routelist->InnerOrderChange_node(n, routeindex);

        if(tmp_routelist->satisfyCapacityConstraint(routeindex, &inputdata)){

          tmp_routelist->ComputeStartTimes(&cost, &inputdata, routeindex);

          // ペナルティの計算
          // tmpPenalty = ALPHA * cost.CalcDistance(tmp_routelist);
          for(int i = 0; i < m; i++){
            // tmpPenalty += BETA * tmp_routelist->min_penalties[i][tmp_routelist->getRouteSize(i) - 2].computeValue(tmp_routelist->start_times[i][tmp_routelist->getRouteSize(i) - 2]);
            for(int j = 1; j < tmp_routelist->getRouteSize(i) - 1; j++){
              if(tmp_routelist->getRoute(i, j) <= n){
                tmpPenalty += BETA * inputdata.getPickupPointer(tmp_routelist->getRoute(i, j))->penalty.computeValue(tmp_routelist->start_times[i][j]);
                if(inputdata.getPickupPointer(tmp_routelist->getRoute(i, j))->maxRideTime < tmp_routelist->ride_times[tmp_routelist->getRoute(i, j) - 1]){
                  tmpPenalty += BETA * (tmp_routelist->ride_times[tmp_routelist->getRoute(i, j) - 1] - inputdata.getPickupPointer(tmp_routelist->getRoute(i, j))->maxRideTime);
                }
              }else{
                tmpPenalty += BETA * inputdata.getDropoffPointer(tmp_routelist->getRoute(i, j) - n)->penalty.computeValue(tmp_routelist->start_times[i][j]);
              }
            }
          }

          cout << tmpPenalty << endl;

          if(tmpPenalty < BestPenalty){
            BestPenalty = tmpPenalty;
            routelist = *tmp_routelist;
          }
        }

        delete tmp_routelist;
      }
    }

    double endtime = cpu_time();



    for(int i = 0; i < m; i++){
      cout << "車両: " << i << endl;
      for(int j = 0; j < routelist.getRouteSize(i); j++){
        cout << "地点: " << routelist.getRoute(i, j) << endl;
        cout << "開始時刻: " << routelist.start_times[i][j] << endl;
        cout << "サービスと次への移動時間: " << routelist.service_ride_times[i][j] << endl;
        if (routelist.getRoute(i, j) <= n) {
          cout << "乗車制約" << inputdata.getPickupPointer(routelist.getRoute(i, j))->maxRideTime << endl;
        }
      }
    }

    //乗車時間の表示
    // for(int i = 0; i < n; i++){
    //   cout << "乗車時間: " << routelist.ride_times[i] << endl;
    // }

    //ルートの表示
    // for(int i = 0; i < m; i++){
    //   cout << "車両: " << i << endl;
    //   for(int j = 0; j < routelist.getRouteSize(i); j++){
    //     cout << routelist.getRoute(i, j) << endl;
    //   }
    // }

    cout << "ペナルティ: " << BestPenalty <<endl;
    // cout << "時間: " << endtime - starttime << "秒" << endl;
}
