cprog: pi.c
	gcc -pthread pi.c -o pi -lm
run: pi
	./pi

part5: part5.cpp
	g++ -pthread -o part5 part5.cpp

runPart5: part5
	./part5
