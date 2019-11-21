a.out :	main.o	InputData.o
	g++ main.o InputData.o
main.o : main.cpp InputData.cpp InputData.h 
	g++ -c main.cpp -o main.o
InputData.o : InputData.cpp InputData.h
	g++ -c InputData.cpp -o InputData.o