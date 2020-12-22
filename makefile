# a.out :	main.o InputData.o myfunction.o Location.o Request.o Cost.o RouteList.o
# 	g++ main.o InputData.o myfunction.o Location.o Request.o Cost.o RouteList.o -I/Library/gurobi900/mac64/include -L/Library/gurobi900/mac64/lib -lgurobi_c++ -lgurobi90
# main.o : main.cpp InputData.hpp Location.hpp Cost.hpp myfunction.hpp RouteList.hpp
# 	g++ -c main.cpp -o main.o -I/Library/gurobi900/mac64/include -L/Library/gurobi900/mac64/lib -lgurobi_c++ -lgurobi90
a.out :	new_main.o InputData.o myfunction.o Location.o Request.o Cost.o RouteList.o PiecewiseLinear.o
	g++ new_main.o InputData.o myfunction.o Location.o Request.o Cost.o RouteList.o PiecewiseLinear.o -std=c++11
new_main.o : new_main.cpp InputData.hpp Location.hpp Cost.hpp myfunction.hpp RouteList.hpp PiecewiseLinear.hpp
	g++ -c new_main.cpp -o new_main.o -std=c++11
InputData.o : InputData.cpp InputData.hpp myfunction.hpp Location.hpp Request.hpp
	g++ -c InputData.cpp -o InputData.o -std=c++11
myfunction.o : myfunction.cpp myfunction.hpp
	g++ -c myfunction.cpp -o myfunction.o -std=c++11
PiecewiseLinear.o : PiecewiseLinear.cpp PiecewiseLinear.hpp
	g++ -c PiecewiseLinear.cpp -o PiecewiseLinear.o -std=c++11
Location.o : Location.hpp Location.cpp
	g++ -c Location.cpp -o Location.o -std=c++11
Request.o : Request.hpp Request.cpp PiecewiseLinear.hpp
	g++ -c Request.cpp -o Request.o -std=c++11
Cost.o : Cost.cpp Cost.hpp RouteList.hpp
	g++ -c Cost.cpp -o Cost.o -std=c++11
RouteList.o : RouteList.hpp RouteList.cpp Cost.hpp PiecewiseLinear.hpp
	g++ -c RouteList.cpp -o RouteList.o -std=c++11
