g++ -c k-means.cpp -o kmeans.o
g++ -c main.cpp -o main.o
g++ kmeans.o main.o -o main
./main
