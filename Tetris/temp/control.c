#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int getch(){
    struct termios tm,old_tm;
    tcgetattr(0,&old_tm);
    cfmakeraw(&tm);
    tcsetattr(0,0,&tm);
    int ch = getchar();
    tcsetattr(0,0,&old_tm);
    return ch;
}

// ^[[A
// 
void key_control(){
    int ch;
    while(1){
        ch = getch();
        if(ch == 'q'){
            break;
        }else if(ch == '\r'){
            printf("down\n");
        }else if(ch == '\33'){
            ch = getch();
            if(ch == '['){
                ch = getchar();
                switch(ch){
                    case 'A':
                        printf("Up\n");
                        break;
                    case 'B':
                        printf("Down\n");
                        break;
                    case 'D':
                        printf("Left\n");
                        break;
                    case 'C':
                        printf("Right\n");
                        break;
                }
            }
        }
    }
}

int main(){
    key_control();
}