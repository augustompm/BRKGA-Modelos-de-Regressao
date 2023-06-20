all: 
	#arguments sequence stackLen,maxConst,populationLen,eliteSize,mutantSize,eliteBias,restartMax,noImprovmentMax,validation
	g++ -g main.cpp -o main -Ofast -std=c++17 ReadIO.cpp Scanner/Scanner.cpp PrintIO.cpp Evaluator.cpp BRKGA.cpp
run: all main
	./main 25 4 100 25 10 85 10 2500 80
valgrind: all
	valgrind --leak-check=full ./main 10 2 30 25 5 70 10 100 90
time: all main
	time ./main 10 2 30 25 5 70 10 100 80
