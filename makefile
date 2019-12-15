a.out :	main.o InputData.o myfunction.o Location.o Request.o Route.o Cost.o RouteList.o PLF.o Solution.o
	g++ main.o InputData.o myfunction.o Location.o Request.o Route.o Cost.o RouteList.o PLF.o Solution.o -I/Library/gurobi900/mac64/include -L/Library/gurobi900/mac64/lib -lgurobi_c++ -lgurobi90
main.o : main.cpp InputData.hpp Route.hpp Location.hpp Cost.hpp myfunction.hpp RouteList.hpp Solution.hpp
	g++ -c main.cpp -o main.o -I/Library/gurobi900/mac64/include -L/Library/gurobi900/mac64/lib -lgurobi_c++ -lgurobi90
InputData.o : InputData.cpp InputData.hpp myfunction.hpp Location.hpp Node.hpp Request.hpp
	g++ -c InputData.cpp -o InputData.o
myfunction.o : myfunction.cpp myfunction.hpp
	g++ -c myfunction.cpp -o myfunction.o
Location.o : Location.hpp Location.cpp
	g++ -c Location.cpp -o Location.o
Node.o : Node.hpp Node.cpp Location.hpp Location.cpp
	g++ -c Node.cpp -o Node.o
Request.o : Request.hpp Request.cpp PLF.hpp
	g++ -c Request.cpp -o Request.o
Route.o : Route.cpp Route.hpp
	g++ -c Route.cpp -o Route.o
Cost.o : Cost.cpp Cost.hpp
	g++ -c Cost.cpp -o Cost.o
RouteList.o : RouteList.hpp RouteList.cpp 
	g++ -c RouteList.cpp -o RouteList.o
PLF.o : PLF.hpp PLF.cpp
	g++ -c PLF.cpp -o PLF.o
Solution.o : Solution.hpp Solution.cpp RouteList.hpp
	g++ -c Solution.cpp -o Solution.o