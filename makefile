CC := g++ -std=c++11
all: network.out switch.out system.out



network.out: main.o networkManager.o 
	$(CC) -o network.out main.o networkManager.o

switch.out: switch.o
	$(CC) -o switch.out switch.o

system.out: system.o
	$(CC) -o system.out system.o




networkManager.o: networkManager.cpp networkManager.hpp
	$(CC) -c networkManager.cpp -o networkManager.o

system.o: system.cpp
	$(CC) -c system.cpp -o system.o

switch.o: switch.cpp
	$(CC) -c switch.cpp -o switch.o

main.o: main.cpp networkManager.hpp 
	$(CC) -c main.cpp -o main.o



.PHONY: clean
clean:
	rm *.o
	rm *.out
	rm *.pipe
