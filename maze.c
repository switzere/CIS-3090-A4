#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <omp.h>


//#define PARALLEL

void serialMaze(int n);
void parallelMaze(int n, int givenX, int givenY);

void init(int n);
void push(int x, int y, int dir);
void pop(int *x, int *y, int *dir);

int getBoardValue(int x, int y);

int inRand(int *arr, int countRand, int n);

/*
0 - up
1 - right
2 - down
3 - left
*/
int stack[1000][3];
int size;

char ** board;


int main(int argc, char **argv) {
  int n = 11;

  int thread_count = 4;

  if(argc == 3) {
    if(strcmp(argv[1],"-n") == 0) {
      printf("%s\n", argv[2]);

      n = strtol(argv[2], NULL, 10);
    }
    else {
      printf("Error reading command\n");
    }
  }

  init(n);

  #ifdef PARALLEL
  # pragma omp parallel num_threads(thread_count)
  parallelMaze(n, 1, 1);
  parallelMaze(n, 1, n-2);
  parallelMaze(n, n-2, 1);
  parallelMaze(n, n-2, n-2);
  #else
  serialMaze(n);
  #endif


  printf("\n\n");

  for(int i = 0; i < n; i++) {
    for(int j = 0; j < n; j++) {
      printf(" %c",board[i][j]);
    }
    printf("\n");
  }

  return 0;
}





void init(int n) {

  for(int i = 0; i < 1000; i++) {
    stack[i][0] = -1;
    stack[i][1] = -1;
    stack[i][2] = -1;
  }

  size = 0;

  board = malloc(sizeof(char*)*n);

  for(int i = 0; i < n; i++) {

    board[i] = malloc(sizeof(char)*n);

    for(int j = 0; j < n; j++) {
        board[i][j] = '.';
    }
  }

}



void push(int x, int y, int dir) {
  size++;
  //printf("push x: %d, y: %d. newSize of: %d\n",x,y,size);
  //printf("size: %d\n",size);
  //printf("x: %d, y: %d\n", x, y);

  for(int i = size; i > 0; i--) {
    //printf("i: %d\n", i);
    stack[i][0] = stack[i-1][0];
    stack[i][1] = stack[i-1][1];
    stack[i][2] = stack[i-1][2];
  }

  stack[0][0] = x;
  stack[0][1] = y;
  stack[0][2] = dir;
}

void pop(int* x, int* y, int* dir) {


  *x = stack[0][0];
  *y = stack[0][1];
  *dir = stack[0][2];

  //printf("pop x: %d, y: %d. newSize of: %d\n", *x, *y, size-1);

  for(int i = 0; i < size-1; i++) {
    stack[i][0] = stack[i+1][0];
    stack[i][1] = stack[i+1][1];
    stack[i][2] = stack[i+1][2];
  }

  size--;
}



int inRand(int *arr, int countRand, int n) {
  for(int i = 0; i < countRand; i++) {
    if(arr[i] == n) {
      return 0;
    }
  }
  return 1;
}



int getBoardValue(int x, int y) {
  return board[x][y];
}

void setBoardValue(int x, int y, int val) {
  board[x][y] = val;
}



void serialMaze(int n) {
  int done = 0;
  int x, y;


  x = 1;
  y = 1;

  printf("SerialMaze\n");
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < n; j++) {
      printf("%c",board[i][j]);
    }
    printf("\n");
  }

  int dir = -1;

  while(done == 0) {


    if(board[x][y] != '1') {

      int countRand = 0;
      int arr[4];
      int r;

      //printf("x: %d,  y: %d,    dir from: %d\n",x,y,dir);

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

      //board[(x+oldX)/2][(y+oldY)/2] = '1';

      for(int i = 0; i < 4; i++) {
        arr[i] = -1;
      }

      while(countRand < 4) {
        //printf("countRand: %d, ",countRand);

        r = rand() % 4;
        //printf("r: %d, ",r);
        if(inRand(arr, countRand, r) == 1) {

          //up
          if(x - 2 > 0 && r == 0) {
            if(board[x-2][y] == '.') {
              push(x-2, y, 0);
            }
          }
          //right
          if(y + 2 < n && r == 1) {
            if(board[x][y+2] == '.') {
              push(x, y+2, 1);
            }
          }
          //down
          if(x + 2 < n && r == 2) {
            if(board[x+2][y] == '.') {
              push(x+2, y, 2);
            }
          }
          //left
          if(y - 2 > 0 && r == 3) {
            if(board[x][y-2] == '.') {
              push(x, y-2, 3);
            }
          }

          arr[countRand] = r;
          countRand++;

        }

      }

    }

      if(size > 0) {
        pop(&x, &y, &dir);
      }
      else {
        done = 1;
      }




    /*  for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
          printf("%c",board[i][j]);
        }
        printf("\n");
      }
      printf("\n");*/

  }


}

void parallelMaze(int n, int givenX, int givenY) {
  int parStack[1000][3];

  for(int i = 0; i < 1000; i++) {
    parStack[i][0] = -1;
    parStack[i][1] = -1;
    parStack[i][2] = -1;
  }



  int my_rank = omp_get_thread_num();
  int thread_count = omp_get_num_threads();
  printf("parallelMaze %d -- Hello from thread %d of %d\n", n, my_rank, thread_count);


  int done = 0;
  int x, y;

  char thread = (my_rank + 1) + '0';

  x = givenX;
  y = givenY;

  #pragma omp critical
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < n; j++) {
      printf("%c",board[i][j]);
    }
    printf("\n");
  }
  int dir = -1;

  while(done == 0) {

      int countRand = 0;
      int arr[4];
      int r;

      //printf("x: %d,  y: %d,    dir from: %d\n",x,y,dir);

      #pragma omp critical
      {

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


      //board[(x+oldX)/2][(y+oldY)/2] = '1';

      for(int i = 0; i < 4; i++) {
        arr[i] = -1;
      }

      while(countRand < 4) {
        //printf("countRand: %d, ",countRand);

        r = rand() % 4;
        //printf("r: %d, ",r);
        if(inRand(arr, countRand, r) == 1) {

          #pragma omp critical
          {
            //up
            if(x - 2 > 0 && r == 0) {
              if(board[x-2][y] == '.') {
                push(x-2, y, 0);
              }
            }
            //right
            if(y + 2 < n && r == 1) {
              if(board[x][y+2] == '.') {
                push(x, y+2, 1);
              }
            }
            //down
            if(x + 2 < n && r == 2) {
              if(board[x+2][y] == '.') {
                push(x+2, y, 2);
              }
            }
            //left
            if(y - 2 > 0 && r == 3) {
              if(board[x][y-2] == '.') {
                push(x, y-2, 3);
              }
            }

          }


          arr[countRand] = r;
          countRand++;

        }

      }



      if(size > 0) {
        int popped = 0;
        while(popped == 0) {
          #pragma omp critical
          {

            pop(&x, &y, &dir);
            if(board[x][y] == '.') {
              popped = 1;
            }

          }
        }

      }
      else {
        done = 1;
      }

      #pragma omp critical
      {
        printf("\n");
        for(int i = 0; i < n; i++) {
          for(int j = 0; j < n; j++) {
            printf("%c",board[i][j]);
          }
          printf("\n");
        }
      }

  }



}
