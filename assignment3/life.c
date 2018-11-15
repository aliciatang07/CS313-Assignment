#include <stdio.h>
#include "life.h"

// When completed this file will contain several versions of the
// life() function and conditional compilation will be used to
// determine which version runs.  Each version will be named life1,
// life2, and so on. To select the version of the life function to
// use, change the following declaration to the version of the life()
// to be used.  All versions of life() must produce a correct result.

// The provided Makefile also includes the possibility of creating an
// executable called "timelife-lifeN" (replacing lifeN with any
// version you'd like to test). You may compile it with "make
// timelife-lifeN" then run "./timelife-lifeN".

#ifndef LIFE_VERSION
#define LIFE_VERSION life4
#endif

// You are only allowed to change the contents of this file with
// respect to improving the performance of this program. You may not
// change the data structure or parameters for the function life.


// If you need to initialize some data structures for your implementation
// of life then make changes to the following function, otherwise 
// do not change this function.

void initLife() {
}

// You are required to document the changes you make in the README.txt
// file. For each entry in the README.txt file there is to be a
// version of the matching life() function here such that the markers
// can see, and run if needed, the actual code that you used to
// produce your results for the logged change to the life function.

static inline int life0(long oldWorld[N][N], long newWorld[N][N]) {
  return base_life(oldWorld, newWorld);
}

// For each version of life you are testing duplicate the function
// below, rename it to a different name, and make your changes. To use
// the new version, change the #define above to use the current version.

static inline int life1(long oldWorld[N][N], long newWorld[N][N]) {
//change each loop from col-major sum to row-major sum 
    int i, j;
    int col, row;
    
    //clear the new world
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++) {
            newWorld[i][j] =  0;
        }
    
    // Count the cells to the top left
    for (i = 0; i < N; i++) {
        row = (i - 1 + N ) % N;
        for (j = 0; j < N; j++) {
            col = (j -1 + N) % N;
            newWorld[i][j] += oldWorld[row][col];
        }
    }
    
    // Count the cells immediately above
    for (i = 0; i < N; i++) {
        row = (i - 1 + N ) % N;
        for (j = 0; j < N; j++) {
            newWorld[i][j] += oldWorld[row][j];
        }
    }
    
    // Count the cells to the top right
    for (i = 0; i < N; i++) {
        row = (i - 1  + N ) % N;
        for (j = 0; j < N; j++) {
             col = (j + 1 + N) % N;
            newWorld[i][j] += oldWorld[row][col];
        }
    }
    
    // Count the cells to the immediate left
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
             col = (j -1 + N) % N;
            newWorld[i][j] += oldWorld[i][col];
        }
    }
    
    // Count the cells to the immediate right
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            col = (j + 1 + N) % N;
            newWorld[i][j] += oldWorld[i][col];
        }
    }
    
    // Count the cells to the bottom left
    for (i = 0; i < N; i++) {
        row = (i + 1 + N ) % N;
        for (j = 0; j < N; j++) {
               col = (j - 1 + N) % N;
            newWorld[i][j] += oldWorld[row][col];
        }
    }
    
    // Count the cells immediately below
    for (i = 0; i < N; i++) {
        row = (i + 1  + N ) % N;
        for (j = 0; j < N; j++) {
            newWorld[i][j] += oldWorld[row][j];
        }
    }
    
    // Count the cells to the bottom right
    for (i = 0; i < N; i++) {
         row = (i + 1  + N ) % N;
        for (j = 0; j < N; j++) {
            col = (j + 1  + N ) % N;
            newWorld[i][j] += oldWorld[row][col];
        }
    }
    
    // Check each cell to see if it should come to life, continue to live, or die
    int alive = 0;
    
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++) {
            newWorld[i][j] = checkHealth(newWorld[i][j], oldWorld[i][j]);
            alive += newWorld[i][j] ? 1:0;
        }
    return alive;
}








static inline int life2(long oldWorld[N][N], long newWorld[N][N]) {
  
int i, j;
int col1, row1;
int col2, row2;

    //combine all the loop together
    int alive =0;
    for (i = 0; i < N; i++) {
        row1 = (i -1 + N) % N;
        row2 = (i +1 + N) % N;
        for (j = 0; j < N; j++) {
        col1 = (j - 1 + N ) % N;
        col2 = (j + 1 + N) % N;
        newWorld[i][j] = oldWorld[row1][col1]+oldWorld[row1][j]+oldWorld[row1][col2]+oldWorld[i][col1]+oldWorld[i][col2]+oldWorld[row2][col1]+oldWorld[row2][j]+oldWorld[row2][col2];
            newWorld[i][j] = checkHealth(newWorld[i][j], oldWorld[i][j]);
            alive += newWorld[i][j]? 1:0;
        }
      }
    return alive;
    
}
// reduce the time of continous reuse between nearby loop
static inline int life3(long oldWorld[N][N], long newWorld[N][N]) {
    
    int alive = 0;
    int i, j;
    int col1, row1;
    int col2, row2;
    int col3,col4;
    int leftup, topup, rightup, left,right,leftdown,bottomdown,rightdown;
    
    for (i = 0; i < N; i++) {
        row1 = (i -1 + N) % N;
        row2 = (i +1 + N) % N;
        for (j = 0; j < N; j+=3) {
            col1 = (j - 1 + N) % N;
            col2 = (j + 1 + N) % N;
            col3 = (j + 2 + N) % N;
            col4 = (j + 3 + N) % N;
            
            //save the recent eight block value
            leftup = oldWorld[row1][col1];
            topup = oldWorld[row1][j];
            rightup = oldWorld[row1][col2];
            left = oldWorld[i][col1];
            right = oldWorld[i][col2];
            leftdown = oldWorld[row2][col1];
            bottomdown = oldWorld[row2][j];
            rightdown = oldWorld[row2][col2];
            
            
            newWorld[i][j] = leftup+topup+rightup+
            left+right+leftdown+bottomdown
            +rightdown;
            newWorld[i][j] = checkHealth(newWorld[i][j], oldWorld[i][j]);
            alive += newWorld[i][j]? 1:0;
            if(j+1<N){
            newWorld[i][j+1] = topup+rightup+oldWorld[row1][col3]
            +oldWorld[i][j]+oldWorld[i][col3]+bottomdown+rightdown
            +oldWorld[row2][col3];
            newWorld[i][j+1] = checkHealth(newWorld[i][j+1], oldWorld[i][j+1]);
            alive += newWorld[i][j+1]? 1:0;
            }
            if(j+2<N){
            newWorld[i][j+2] = rightup+oldWorld[row1][col3]+oldWorld[row1][col4]
            +right+oldWorld[i][col4]+rightdown+oldWorld[row2][col3]
            +oldWorld[row2][col4];
            newWorld[i][j+2] = checkHealth(newWorld[i][j+2], oldWorld[i][j+2]);
            alive += newWorld[i][j+2]? 1:0;
            }
        }
    }
    return alive;
}
// add more local variable to increase temporal locality
static inline int life4(long oldWorld[N][N], long newWorld[N][N]) {
    
    int alive = 0;
    int i, j;
    int col1, row1;
    int col2, row2;
    int col3,col4;
    int row3,row4;
    int leftup, topup, rightup, left,right,leftdown,bottomdown,rightdown;
    int reusetop, reusedown;
    int reuse_i2_row3col1,reuse_i3_row3j,reuse_i1_topleft,reuse_i3_row3col2,reuse_i3_row3col3,reuse_i3_icol3;
    for (i = 0; i < N; i+=3) {
        row1 = (i -1 + N) % N;
        row2 = (i + 1 + N) % N;
        row3 = (i + 2 + N) % N;
        row4 = (i + 3 + N) % N;
        for (j = 0; j < N; j+=3) {
            col1 = (j - 1 + N) % N;
            col2 = (j + 1 + N) % N;
            col3 = (j + 2 + N) % N;
            col4 = (j + 3 + N) % N;
            
            //save the recent eight block value
            //create local variable to save the element that can be reused 
            leftup = oldWorld[row1][col1];
            topup = oldWorld[row1][j];
            rightup = oldWorld[row1][col2];
            left = oldWorld[i][col1];
            right = oldWorld[i][col2];
            leftdown = oldWorld[row2][col1];
            bottomdown = oldWorld[row2][j];
            rightdown = oldWorld[row2][col2];
            reusetop = oldWorld[row1][col3];
            reusedown = oldWorld[row2][col3];
            reuse_i1_topleft =oldWorld[i][j];
            reuse_i2_row3col1 = oldWorld[row3][col1];
            reuse_i3_row3j = oldWorld[row3][j];
            reuse_i3_row3col2 = oldWorld[row3][col2];
            reuse_i3_row3col3 = oldWorld[row3][col3];
            reuse_i3_icol3 = oldWorld[i][col3];
            
            //IJ
            newWorld[i][j] = leftup+topup+rightup+
            left+right+leftdown+bottomdown
            +rightdown;
            newWorld[i][j] = checkHealth(newWorld[i][j], oldWorld[i][j]);
            alive += newWorld[i][j]? 1:0;
            //IJ+1
            if(j+1<N){
                newWorld[i][j+1] = topup+rightup+reusetop
                +oldWorld[i][j]+reuse_i3_icol3+bottomdown+rightdown
                +reusedown;
                newWorld[i][j+1] = checkHealth(newWorld[i][j+1], oldWorld[i][j+1]);
                alive += newWorld[i][j+1]? 1:0;
            }
            //IJ+2
            if(j+2<N){
                newWorld[i][j+2] = rightup+reusetop+oldWorld[row1][col4]
                +right+oldWorld[i][col4]+rightdown+reusedown
                +oldWorld[row2][col4];
                newWorld[i][j+2] = checkHealth(newWorld[i][j+2], oldWorld[i][j+2]);
                alive += newWorld[i][j+2]? 1:0;
            }
            //I+1J
            if(i+1<N){
            newWorld[i+1][j] = oldWorld[i][col1]+reuse_i1_topleft+ right+leftdown+rightdown+reuse_i2_row3col1+reuse_i3_row3j+reuse_i3_row3col2;
            newWorld[i+1][j] = checkHealth(newWorld[i+1][j], oldWorld[i+1][j]);
            alive += newWorld[i+1][j]? 1:0;
            
            //I+1J+1
            if(j+1<N){
            newWorld[i+1][j+1] =reuse_i1_topleft+ right+reuse_i3_icol3+bottomdown+reusedown+reuse_i3_row3j+reuse_i3_row3col2+reuse_i3_row3col3;
                newWorld[i+1][j+1] = checkHealth(newWorld[i+1][j+1], oldWorld[i+1][j+1]);
                alive += newWorld[i+1][j+1]? 1:0;
            }
            //I+1J+2
            if(j+2<N){
            newWorld[i+1][j+2] = right+reuse_i3_icol3+oldWorld[i][col4]+rightdown+oldWorld[row2][col4]+reuse_i3_row3col2+reuse_i3_row3col3+oldWorld[row3][col4];
                newWorld[i+1][j+2] = checkHealth(newWorld[i+1][j+2], oldWorld[i+1][j+2]);
                alive += newWorld[i+1][j+2]? 1:0;
                }
            }
            //I+2J
            if(i+2<N){
            newWorld[i+2][j] = leftdown+bottomdown+rightdown+reuse_i2_row3col1+reuse_i3_row3col2+oldWorld[row4][col1]+oldWorld[row4][j]+oldWorld[row4][col2];
            newWorld[i+2][j] = checkHealth(newWorld[i+2][j], oldWorld[i+2][j]);
            alive += newWorld[i+2][j]? 1:0;
                
            //I+2J+1
            if(j+1<N){
            newWorld[i+2][j+1] = bottomdown+oldWorld[row2][col2]+reusedown+reuse_i3_row3j+reuse_i3_row3col3+oldWorld[row4][j]+oldWorld[row4][col2]+oldWorld[row4][col3];
                newWorld[i+2][j+1] = checkHealth(newWorld[i+2][j+1], oldWorld[i+2][j+1]);
                alive += newWorld[i+2][j+1]? 1:0;
            }
            //I+2J+2
            if(j+2<N){
          newWorld[i+2][j+2] = oldWorld[row2][col2]+reusedown+oldWorld[row2][col4]+reuse_i3_row3col2+oldWorld[row3][col4]+oldWorld[row4][col2]+oldWorld[row4][col3]+oldWorld[row4][col4];
                newWorld[i+2][j+2] = checkHealth(newWorld[i+2][j+2], oldWorld[i+2][j+2]);
                alive += newWorld[i+2][j+2]? 1:0;
                }
            }
        }
    }
    return alive;
}

int life(long oldWorld[N][N], long newWorld[N][N]) {
  return LIFE_VERSION(oldWorld, newWorld);
}
