#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

vector<double> splitdouble(string input, char delimiter){
    istringstream stream(input);
    double temp;
	string field;
	vector<double> result;

	while (getline(stream, field, delimiter))
	{
		try {
			temp = stod(field);
		}
		catch (const std::invalid_argument& e) {
			// std::cout << e.what(); // "stod: no conversion"
			std::cout << "Error: cannot convert to double.";
		}
		result.push_back(temp);
	}
    return result;
}

struct Function{
  int lower;
  int upper;
  double slope;
  double intercept;
  Function *next;
};

class PiecewiseLinear{
public:
  Function *head;
  PiecewiseLinear();
  void appendFunction(int lower, int upper, double slope, double intercept);
  void displayFunctions();
  PiecewiseLinear* shiftToRight(int diff);
  PiecewiseLinear* minimize();
  PiecewiseLinear* copy(PiecewiseLinear* origin);
  PiecewiseLinear* sum(PiecewiseLinear* plf_1, PiecewiseLinear* plf_2);
};

PiecewiseLinear::PiecewiseLinear(){
  head = NULL;
}

void PiecewiseLinear::appendFunction(int lower, int upper, double slope, double intercept){
  Function *new_func;
  Function *tmp;

  new_func = new Function;
  new_func->lower = lower;
  new_func->upper = upper;
  new_func->slope = slope;
  new_func->intercept = intercept;
  new_func->next = NULL;

  if(!head){
    head = new_func;
  }else{
    tmp = head;
    while(tmp->next){
      tmp = tmp->next;
    }
    tmp->next = new_func;
  }
}

void PiecewiseLinear::displayFunctions(){
  if(head == NULL){
    cout << "no function." << endl;
  }else{
    Function* tmp = head;

    while(tmp){
      cout << "lower: " << tmp->lower << endl;
      cout << "upper: " << tmp->upper << endl;
      cout << "slope: " << tmp->slope << endl;
      cout << "intercept: " << tmp->intercept << endl;
      tmp = tmp->next;
    }
  }
}

PiecewiseLinear* PiecewiseLinear::shiftToRight(int diff){
  Function* tmp = this->head;

  while(tmp){
    if(tmp->upper != 480){
      tmp->upper += diff;
    }
    tmp->intercept += -diff * tmp->slope;
    if(tmp->lower != 0){
      tmp->lower += diff;
    }
    tmp = tmp->next;
  }

  return this;
}

PiecewiseLinear* PiecewiseLinear::minimize(){
  Function* tmp = this->head;
  double min;

  while(tmp->slope <= 0){
    tmp = tmp->next;
  }

  min = tmp->slope * tmp->lower + tmp->intercept;

  while(tmp){
    tmp->intercept = min;
    tmp->slope = 0;
    tmp = tmp->next;
  }

  return this;
}

PiecewiseLinear* PiecewiseLinear::copy(PiecewiseLinear* origin){
  Function* ori_plf_func = origin->head;

  while(ori_plf_func){
    appendFunction(ori_plf_func->lower, ori_plf_func->upper, ori_plf_func->slope, ori_plf_func->intercept);
    ori_plf_func = ori_plf_func->next;
  }

  return this;
}

PiecewiseLinear* PiecewiseLinear::sum(PiecewiseLinear* plf_1, PiecewiseLinear* plf_2){
  Function* plf_1_func = plf_1->head;
  Function* plf_2_func = plf_2->head;

  while(plf_1_func || plf_2_func){
    appendFunction(max(plf_1_func->lower, plf_2_func->lower), min(plf_1_func->upper, plf_2_func->upper), plf_1_func->slope + plf_2_func->slope, plf_1_func->intercept + plf_2_func->intercept);

    if(plf_1_func->upper < plf_2_func->upper){
      plf_1_func = plf_1_func->next;
    }else if(plf_1_func->upper > plf_2_func->upper){
      plf_2_func = plf_2_func->next;
    }else{
      plf_1_func = plf_1_func->next;
      plf_2_func = plf_2_func->next;
    }
  }

  return this;
}


//　sample_instanceのペナルティ関数を用いて、サービス時間と移動時間の和を2として, 増田ソルバーを実行
int main(){
  vector<int> route = {0, 2, 1, 3};
  vector<PiecewiseLinear> penalties;
  vector<double> temp_double_v;
  vector<PiecewiseLinear> min_penalties;
  vector<int> start_times;
  int n;

  ifstream file("./sample_instance");
  n = 4;
  penalties.resize(n);
  min_penalties.resize(n);
  start_times.resize(n);

  // ペナルティ関数をセット
  string line;

  for(int i = 0; i < n; i++){
    getline(file,line);
    temp_double_v = splitdouble(line,',');

    for(int j = 0; j < temp_double_v.size() / 4; j++){
      penalties[i].appendFunction(temp_double_v[j * 4], temp_double_v[j * 4 + 1], temp_double_v[j * 4 + 2], temp_double_v[j * 4 + 3]);
    }
  }
  temp_double_v.clear();

  // DP
  for(int i = 0; i < route.size(); i++){
    if(i == 0){
      min_penalties[0].copy(&penalties[route[i]]);
    }else{
      min_penalties[i].sum(min_penalties[i - 1].shiftToRight(2), &penalties[route[i]]);
      min_penalties[i].minimize();
    }
  }

  // 開始時刻の決定
  for(int i = route.size() - 1; i >= 0; i--){
    Function* tmp_func = min_penalties[i].head;
    int opt_time = 0;
    double min_penalty = min_penalties[i].head->intercept;
    double tmp_penalty;

    if(i == route.size() - 1){
      while(tmp_func){
        tmp_penalty = tmp_func->slope * tmp_func->upper + tmp_func->intercept;
        if(min_penalty > tmp_penalty){
          min_penalty = tmp_penalty;
          opt_time = tmp_func->upper;
        }
        tmp_func = tmp_func->next;
      }
    }else{
      while(tmp_func){
        int upper_bound = start_times[i + 1] - 2;
        tmp_penalty = tmp_func->slope * tmp_func->upper + tmp_func->intercept;
        if(min_penalty > tmp_penalty){
          if(tmp_func->upper <= upper_bound){
            min_penalty = tmp_penalty;
            opt_time = tmp_func->upper;
          }else if(tmp_func->slope * upper_bound + tmp_func->intercept < min_penalty){
            min_penalty = tmp_func->slope * upper_bound + tmp_func->intercept;
            opt_time = upper_bound;
          }
        }
        tmp_func = tmp_func->next;
      }
    }
    start_times[i] = opt_time;
  }

  for(int i = 0; i < route.size(); i++){
    cout << route[i] << "の開始時刻: " << start_times[i] << endl;
  }
}
