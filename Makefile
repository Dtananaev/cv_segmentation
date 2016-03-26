project: kmeans.cpp Makefile
	g++ -std=c++11 kmeans.cpp NMath.cpp -I. -o kmeans
clean:
	rm kmeans
