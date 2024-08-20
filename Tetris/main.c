#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <signal.h>
#include "user_print.h"


int next_num = 0;
int next_mode = 0;
int next_color = 0;

int init_x = 24;
int init_y = 6;

int next_x = 46;
int next_y = 8;

int dynamic_x = 0;
int dynamic_y = 0;

int dynamic_num = 0;
int dynamic_mode = 0;
int dynamic_color = 0;

int tm = 800000; // 0.8s
int matrix[24][28];

 int shape[7][4][18] = {
        {                                 
            {1,1,0,0, 1,1,0,0, 0,0,0,0, 0,0,0,0, 2,2},      //
            {1,1,0,0, 1,1,0,0, 0,0,0,0, 0,0,0,0, 2,2},      //  [][]
            {1,1,0,0, 1,1,0,0, 0,0,0,0, 0,0,0,0, 2,2},      //  [][]
            {1,1,0,0, 1,1,0,0, 0,0,0,0, 0,0,0,0, 2,2}       //             
        },
        {
            {1,1,1,1, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,3},  //  [][][][]  []
            {1,0,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0, 3,0},  //            []
            {1,1,1,1, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,3},  //            []
            {1,0,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0, 3,0}   //            []
        },
        {
            {0,1,0,0, 1,1,1,0, 0,0,0,0, 0,0,0,0, 1,2},  //  []      []      [][][]     []
            {1,0,0,0, 1,1,0,0, 1,0,0,0, 0,0,0,0, 2,1},  //[][][]    [][]      []     [][]
            {1,1,1,0, 0,1,0,0, 0,0,0,0, 0,0,0,0, 1,2},  //          []                 []
            {0,1,0,0, 1,1,0,0, 0,1,0,0, 0,0,0,0, 2,1}   //
        },
        {
            {1,1,0,0, 0,1,1,0, 0,0,0,0, 0,0,0,0, 1,2},   //  [][]        []
            {0,1,0,0, 1,1,0,0, 1,0,0,0, 0,0,0,0, 2,1},   //    [][]    [][]
            {1,1,0,0, 0,1,1,0, 0,0,0,0, 0,0,0,0, 1,2},   //            []
            {0,1,0,0, 1,1,0,0, 1,0,0,0, 0,0,0,0, 2,1}
        },
        {
            {0,1,1,0, 1,1,0,0, 0,0,0,0, 0,0,0,0, 1,2},   //    [][]    []
            {1,0,0,0, 1,1,0,0, 0,1,0,0, 0,0,0,0, 2,1},   //  [][]      [][]
            {0,1,1,0, 1,1,0,0, 0,0,0,0, 0,0,0,0, 1,2},   //              []
            {1,0,0,0, 1,1,0,0, 0,1,0,0, 0,0,0,0, 2,1}    //
        },
        {   
            {1,0,0,0, 1,1,1,0, 0,0,0,0, 0,0,0,0, 1,2},   //   []       [][]   [][][]    []
            {1,1,0,0, 1,0,0,0, 1,0,0,0, 0,0,0,0, 2,1},   //   [][][]   []         []    []
            {1,1,1,0, 0,0,1,0, 0,0,0,0, 0,0,0,0, 1,2},   //            []             [][]
            {0,1,0,0, 0,1,0,0, 1,1,0,0, 0,0,0,0, 2,1}    //
        },
        {
            {0,0,1,0, 1,1,1,0, 0,0,0,0, 0,0,0,0, 1,2},    //     []   []      [][][]  [][]
            {1,0,0,0, 1,0,0,0, 1,1,0,0, 0,0,0,0, 2,1},    // [][][]   []      []        []
            {1,1,1,0, 1,0,0,0, 0,0,0,0, 0,0,0,0, 1,2},    //          [][]              []
            {1,1,0,0, 0,1,0,0, 0,1,0,0, 0,0,0,0, 2,1}     //
        }
    };

int score_x = 45;  // score and level locations
int score_y = 18;
int level_x = 45;
int level_y = 22;
int user_score = 0;

struct termios old_tm;
int getch(){
    struct termios tm;
    tcgetattr(0,&old_tm);
    cfmakeraw(&tm);
    tcsetattr(0,0,&tm);
    int ch = getchar();
    tcsetattr(0,0,&old_tm);
    return ch;
}
void recover_attribute(){
   tcsetattr(0,0,&old_tm); 
}

void game_over()
{
        printf("\33[32;9H**********  Game Over  ********\33[0m");
        //光标显示
        printf("\33[?25h");
        printf("\n\n");
}

int change_shape(){
    int m = (dynamic_mode + 1) % 4;
    //右侧越界检测
    if(dynamic_x + 2 * (4 - shape[dynamic_num][dynamic_mode][16]) - 1 > 39)
        return 1;
    if(dynamic_y + (4 - shape[dynamic_num][dynamic_mode][17]) - 1 > 29)
        return 1;
    eraser_shape(dynamic_num,dynamic_mode,dynamic_x,dynamic_y);
    dynamic_mode = m;
    print_mode_shape(dynamic_num,dynamic_mode,dynamic_x,dynamic_y,dynamic_color);
    return 0;
}

void store_current_shape(){
    int m_line = dynamic_y - 6;
    int m_column = dynamic_x - 12;

    for(int i = 0;i<16;i++){
        if(i % 4 == 0 && i != 0){
            m_line++;
            m_column = dynamic_x - 12;
        }

        if(shape[dynamic_num][dynamic_mode][i] == 1){
            matrix[m_line][m_column] = dynamic_color;
            matrix[m_line][m_column + 1] = dynamic_color;
        }

        m_column += 2;
    }
}

int judge_shape(int num,int mode,int x,int y){
    int m_line = y - 6;
    int m_column = x - 12;

    for(int i = 0;i<16;i++){
        if(i != 0 && i % 4 == 0){
            m_line ++;
            m_column = x - 12;
        }

        if(shape[num][mode][i] == 1){
            if(matrix[m_line][m_column] != 0){
                return 1;
            }
        }
        m_column += 2;
    }
    return 0;
}

void init_new_shape(){
    dynamic_num = next_num;
    dynamic_mode = next_mode;
    dynamic_color = next_color;

    dynamic_x = init_x;
    dynamic_y = init_y;

    print_mode_shape(dynamic_num,dynamic_mode,dynamic_x,dynamic_y,dynamic_color);
}

void print_matrix(){
    for(int i = 0;i<24;i++){
        for(int j = 0;j<28;j+=2){
            if(matrix[i][j] == 0){
                printf("\033[%d;%dH  \033[0m",i + 6,j + 12);
            }else{
                printf("\033[%d;%dH\033[%dm[]\033[0m",i + 6,j + 12,matrix[i][j]);
            }
        }
    }
}

//输出分数 
void print_score_level()
{
        printf("\33[%d;%dH分数:%d\33[0m",score_y,score_x,user_score);
        fflush(NULL);
}

void destory_cond_line(){
    int flag = 0;
    for(int i = 0;i<24;i++){
        flag = 1;
        for(int j = 0;j<28;j++){
            if(matrix[i][j] == 0){
                flag = 0;
                break;
            }
        }

        // 某一行满了
        if(flag == 1){
            user_score += 10;

            for(int k = i;k > 0;k--){
                for(int l = 0;l<28;l++){
                    matrix[k][l] = matrix[k-1][l];
                }

                print_matrix();
                print_score_level();
            }
        }
    }
}

int move_down(int n,int m){
   if((dynamic_y + (4 - shape[n][m][17]) - 1 >= 29) || judge_shape(n,m,dynamic_x,dynamic_y+1)){
        //store and stop
        store_current_shape();
        init_new_shape();
        destory_cond_line();
        print_next_shape();
        return 1; 
   }

   eraser_shape(n,m,dynamic_x,dynamic_y); 
   dynamic_y += 1;
   print_mode_shape(n,m,dynamic_x,dynamic_y,dynamic_color);
   return 0;
}

int move_left(int n,int m){
   if(dynamic_x <= 12){
    return 1;
   }
   if(judge_shape(dynamic_num,dynamic_mode,dynamic_x-2,dynamic_y)){
    return 1;
   }
   eraser_shape(n,m,dynamic_x,dynamic_y); 
   dynamic_x -= 2;
   print_mode_shape(n,m,dynamic_x,dynamic_y,dynamic_color); 
   return 0;
}

int move_right(int n,int m){
    if(dynamic_x + 2 * (4 - shape[n][m][16]) - 1 >= 39){
    return 1;
   }
   if(judge_shape(dynamic_num,dynamic_mode,dynamic_x+2,dynamic_y)){
    return 1;
   }
   eraser_shape(n,m,dynamic_x,dynamic_y); 
   dynamic_x += 2;
   print_mode_shape(n,m,dynamic_x,dynamic_y,dynamic_color); 
   return 0;
}

void fall_down(){
    while(1){
        int ret = move_down(dynamic_num,dynamic_mode);
        if(ret == 1){
            return;
        }
    }
}


void key_control(){
    int ch;
    while(1){
        ch = getch();
        if(ch == 'q'){
            break;
        }else if(ch == '\r'){
            fall_down();
        }else if(ch == '\33'){
            ch = getch();
            if(ch == '['){
                ch = getchar();
                switch(ch){
                    case 'A':
                        change_shape();
                        break;
                    case 'B':
                        move_down(dynamic_num,dynamic_mode);
                        break;
                    case 'D':
                        move_left(dynamic_num,dynamic_mode);
                        break;
                    case 'C':
                        move_right(dynamic_num,dynamic_mode);
                        break;
                    default:
                        break;
                }
            }
        }
    }
    game_over();
}

void print_start_ui(){
    printf("\033[2J");
    
    // top and bottom border
    for(int i = 0;i<47;i++){
        printf("\033[%d;%dH\033[43m \033[0m",5,i+10);
        printf("\033[%d;%dH\033[43m \033[0m",30,i+10); 
    }

    // vertical border
    for(int i = 0;i<26;i++){
        printf("\033[%d;%dH\033[43m  \033[0m",i+5,10);
        printf("\033[%d;%dH\033[43m  \033[0m",i+5,40);
        printf("\033[%d;%dH\033[43m  \033[0m",i+5,56); 
    }

    // cut user panel and score
    for(int i = 0;i<17;i++){
        printf("\033[%d;%dH\033[43m \033[0m",12,40 + i);
    }

    printf("\033[%d;%dHMark:\033[0m",score_y,score_x);
    printf("\033[%d;%dHLevel:\033[0m",level_y,level_x); 

    fflush(NULL);
}

void init_game_ui(){
    print_start_ui();
    getch();
    srand(time(NULL));

    dynamic_num = random() % 7;
    dynamic_mode = random() % 4;
    dynamic_color = random() % 7 + 40;

    dynamic_x = init_x;
    dynamic_y = init_y;

    print_mode_shape(dynamic_num,dynamic_mode,dynamic_x,dynamic_y,dynamic_color);
    print_next_shape();
    printf("\033[?25l");
}

int get_martix_result(int n_line){
    if (n_line < 0)
    {
        return 0;
    }
    for(int i = 0;i<28;i++){
        if(matrix[n_line][i] != 0)
            return 1;
    }
    return 0;
}
int judge_end_game(){
    int n_line = 23;
    for(int i = 0;i<24;i++){
        if(get_martix_result(n_line))
            n_line--;
        else
            return 0;
    }
    return 1;
}

void sig_handler(int signum){

    move_down(dynamic_num,dynamic_mode);

    if(judge_end_game() == 1){
        game_over();
        recover_attribute();
        exit(0);
    }
    fflush(NULL);
}
void alarm_us(int n){
    struct itimerval value;

    //设置定时器启动的初始值n
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = n;

    //设置定时器启动后的间隔时间值n
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = n;

    setitimer(ITIMER_REAL,&value,NULL);
}



int main(){

    init_game_ui(); 
    signal(SIGALRM,sig_handler);
    alarm_us(tm);

    key_control();
    return 0;
}
