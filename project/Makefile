all:
	mpicc main.c func.c main.h -o main -fopenmp -lm

run:
	mpirun -np 3 main

clean:
	rm main
