a.out :	main.o InputData.o myfunction.o Location.o Node.o
	g++ main.o InputData.o myfunction.o Location.o Node.o
main.o : main.cpp InputData.cpp InputData.cpp Location.cpp Node.cpp
	g++ -c main.cpp -o main.o
InputData.o : InputData.cpp InputData.hpp myfunction.hpp myfunction.cpp Location.cpp Location.hpp Node.cpp Node.hpp
	g++ -c InputData.cpp -o InputData.o
myfunction.o : myfunction.cpp myfunction.hpp
	g++ -c myfunction.cpp -o myfunction.o
Location.o : Location.hpp Location.cpp
	g++ -c Location.cpp -o Location.o
Node.o : Node.hpp Node.cpp Location.hpp Location.cpp
	g++ -c Node.cpp -o Node.o