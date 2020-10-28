all: maze

maze: maze.c
	gcc -g maze.c -o maze -fopenmp

mazePar: mazePar.c
	gcc -g mazePar.c -o mazePar -fopenmp
