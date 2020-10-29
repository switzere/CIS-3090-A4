#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <omp.h>

void serialMaze(int n);
void parallelMaze(int n);

void init(int n);
void push(int x, int y, int dir, int stack[10000][3], int *size);
void pop(int *x, int *y, int *dir, int stack[10000][3], int *size);

int inRand(int *arr, int countRand, int n);

/*
Directions
0 - up
1 - right
2 - down
3 - left
*/

char ** board;

int countArr[4];


//TODO -s what does he mean by seed?

int main(int argc, char **argv) {
  int n = 11;
  int seed = -1;
  int argPtr;

  int thread_count = 4;

  if(argc > 1) {
    argPtr = 1;
    while(argPtr < argc) {
      if(strcmp(argv[argPtr], "-n") == 0) {
        n = strtol(argv[argPtr+1], NULL, 10);
        argPtr += 2;
      }
      else if(strcmp(argv[argPtr], "-s") == 0) {
        seed = strtol(argv[argPtr+1], NULL, 10);
        srand(seed);
        argPtr += 2;
      }
      else {
        printf("USAGE: %s <-n size of board> <-s seed>\n", argv[0]);
        exit(1);
      }
    }
  }

/*
  if(argc == 3) {
    if(strcmp(argv[1],"-n") == 0) {
      n = strtol(argv[2], NULL, 10);
    }
    else {
      printf("Error reading command\n");
    }
  }*/

  init(n);

  #ifdef PARALLEL
  # pragma omp parallel num_threads(thread_count)
  parallelMaze(n);
  #else
  serialMaze(n);
  #endif


  for(int i = 0; i < n; i++) {
    for(int j = 0; j < n; j++) {
      printf(" %c",board[i][j]);
    }
    printf("\n");
  }

  #ifdef PARALLEL
  for(int i = 0; i < 4; i++) {
    printf("process %d count: %d\n", i, countArr[i]);
  }
  #endif


  return 0;
}




/*
Initialize board and countArr
*/
void init(int n) {

  for(int i = 0; i < 4; i++) {
    countArr[i] = 0;
  }

  board = malloc(sizeof(char*)*n);

  for(int i = 0; i < n; i++) {

    board[i] = malloc(sizeof(char)*n);

    for(int j = 0; j < n; j++) {
        board[i][j] = '.';
    }
  }

}


/*
Push location (x,y) with a direction onto the stack
*/
void push(int x, int y, int dir, int stack[10000][3], int* size) {
  *size = *size + 1;

  for(int i = *size; i > 0; i--) {
    stack[i][0] = stack[i-1][0];
    stack[i][1] = stack[i-1][1];
    stack[i][2] = stack[i-1][2];
  }

  stack[0][0] = x;
  stack[0][1] = y;
  stack[0][2] = dir;
}

/*
Pop location (x,y) and direction from stack
*/
void pop(int* x, int* y, int* dir, int stack[10000][3], int* size) {
  *x = stack[0][0];
  *y = stack[0][1];
  *dir = stack[0][2];

  for(int i = 0; i < *size-1; i++) {
    stack[i][0] = stack[i+1][0];
    stack[i][1] = stack[i+1][1];
    stack[i][2] = stack[i+1][2];
  }

  *size = *size - 1;
}


/*
Check if random number is in array
*/
int inRand(int *arr, int countRand, int n) {
  for(int i = 0; i < countRand; i++) {
    if(arr[i] == n) {
      return 0;
    }
  }
  return 1;
}


/*
Create maze in serial
*/
void serialMaze(int n) {
  int stack[10000][3];
  int size = 0;
  int done = 0;
  int x, y;





  x = 1;
  y = 1;


  int dir = -1;

  //While more locations on stack
  while(done == 0) {

    //if location is still available
    if(board[x][y] != '1') {

      int countRand = 0;
      int arr[4];
      int r;

      board[x][y] = '1';

      if(dir == 0) {
        board[x+1][y] = '1';
      }
      else if(dir == 1) {
        board[x][y-1] = '1';
      }
      else if(dir == 2) {
        board[x-1][y] = '1';
      }
      else if(dir == 3) {
        board[x][y+1] = '1';
      }

      //init array
      for(int i = 0; i < 4; i++) {
        arr[i] = -1;
      }

      //choose order of directions randomly
      while(countRand < 4) {

        r = rand() % 4;
        if(inRand(arr, countRand, r) == 1) {

          //up
          if(x - 2 > 0 && r == 0) {
            if(board[x-2][y] == '.') {
              push(x-2, y, 0, stack, &size);
            }
          }
          //right
          if(y + 2 < n && r == 1) {
            if(board[x][y+2] == '.') {
              push(x, y+2, 1, stack, &size);
            }
          }
          //down
          if(x + 2 < n && r == 2) {
            if(board[x+2][y] == '.') {
              push(x+2, y, 2, stack, &size);
            }
          }
          //left
          if(y - 2 > 0 && r == 3) {
            if(board[x][y-2] == '.') {
              push(x, y-2, 3, stack, &size);
            }
          }

          arr[countRand] = r;
          countRand++;

        }

      }

    }

    //If more on stack
    if(size > 0) {
      pop(&x, &y, &dir, stack, &size);
    }
    else {
      done = 1;
    }

  }

}



/*
Create maze in parallel
*/
void parallelMaze(int n) {
  int parStack[10000][3];
  int size = 0;


  for(int i = 0; i < 10000; i++) {
    parStack[i][0] = -1;
    parStack[i][1] = -1;
    parStack[i][2] = -1;
  }

  int my_rank = omp_get_thread_num();


  int done = 0;
  int x, y;

  //Set starting position
  if(my_rank == 0) {
    x = 1;
    y = 1;
  }
  else if(my_rank == 1) {
    x = n - 2;
    y = 1;
  }
  else if(my_rank == 2) {
    x = n - 2;
    y = n - 2;
  }
  else if(my_rank == 3) {
    x = 1;
    y = n - 2;
  }

  char thread = my_rank + '0';

  int dir = -1;
  int placedFlag = 0;

  //While more locations on stack
  while(done == 0) {

    int countRand = 0;
    int arr[4];
    int r;

    #pragma omp critical
    {

      //if location is still available
      if(board[x][y] == '.') {

        if(placedFlag != 0) {
          //placed
          countArr[my_rank]++;
        }
        else {
          //not placed yet
          placedFlag = 1;
        }

        board[x][y] = thread;

        if(dir == 0) {
          board[x+1][y] = thread;
        }
        else if(dir == 1) {
          board[x][y-1] = thread;
        }
        else if(dir == 2) {
          board[x-1][y] = thread;
        }
        else if(dir == 3) {
          board[x][y+1] = thread;
        }

      }
      else {
        //If spot taken, dont push any neighbours
        countRand = 4;
      }

    }


    for(int i = 0; i < 4; i++) {
      arr[i] = -1;
    }

    //choose order of directions randomly
    while(countRand < 4) {

      r = rand() % 4;

      if(inRand(arr, countRand, r) == 1) {

        #pragma omp critical
        {
          //up
          if(x - 2 > 0 && r == 0) {
            if(board[x-2][y] == '.') {
              push(x-2, y, 0, parStack, &size);
            }
          }
          //right
          if(y + 2 < n && r == 1) {
            if(board[x][y+2] == '.') {
              push(x, y+2, 1, parStack, &size);
            }
          }
          //down
          if(x + 2 < n && r == 2) {
            if(board[x+2][y] == '.') {
              push(x+2, y, 2, parStack, &size);
            }
          }
          //left
          if(y - 2 > 0 && r == 3) {
            if(board[x][y-2] == '.') {
              push(x, y-2, 3, parStack, &size);
            }
          }

        }


        arr[countRand] = r;
        countRand++;

      }

    }


    //If more on stack
    if(size > 0) {
      int popped = 0;
      //pop until a free spot is found
      while(popped == 0) {
        #pragma omp critical
        {
          pop(&x, &y, &dir, parStack, &size);
          if(board[x][y] == '.') {
            popped = 1;
          }
          else {
            if(size == 0) {
              done = 1;
              popped = 1;
            }
          }

        }
      }

    }
    else {
      done = 1;
    }

  }

}
