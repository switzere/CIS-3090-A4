all: maze mazep

maze: maze.c
	gcc -g maze.c -o maze -fopenmp

mazep: maze.c
	gcc -g -DPARALLEL maze.c -o mazep -fopenmp
