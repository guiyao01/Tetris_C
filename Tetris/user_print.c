#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <signal.h>
#include "user_print.h"

// n 图案
// m 方向
// x,y 坐标
// c 颜色
void print_mode_shape(int n,int m,int x,int y,int c){
    int xx = x;
    int yy = y;
    printf("\033[?25l");
    for(int i = 0;i<16;i++){
        if(i % 4 == 0 && i != 0){
            yy++;
            xx = x;
        }
        if(shape[n][m][i] == 1){
            printf("\033[%d;%dH",yy,xx);
            printf("\033[%dm[]",c);
            printf("\033[0m");
        }
        xx+=2;  
    }
    fflush(NULL);
}

void eraser_shape(int n,int m,int x,int y){
    int xx = x;
    int yy = y;
    for(int i = 0;i < 16;i++){
        if(i % 4 == 0 && i != 0){
            yy ++;
            xx = x;
        }
        if(shape[n][m][i] == 1){
            printf("\033[%d;%dH  \033[0m",yy,xx);
        }
        xx += 2;
    }
    fflush(NULL);
}

void print_next_shape(){
    eraser_shape(next_num,next_mode,next_x,next_y);
    next_num = random() % 7;
    next_mode = random() % 4;
    next_color = (random() % 7) + 40;

    print_mode_shape(next_num,next_mode,next_x,next_y,next_color);
    fflush(NULL);
}