#ifndef __USER_PRINT_H__
#define __USER_PRINT_H__

extern int next_num;
extern int next_mode;
extern int next_color;

// next tiny square location
extern int next_x;
extern int next_y;

//initial tiny square location
extern int init_x;
extern int init_y;

// current attribute
extern int dynamic_num;
extern int dynamic_mode;
extern int dynamic_color;

//current location
extern int dynamic_x;
extern int dynamic_y;

extern int shape[7][4][18];

extern void print_mode_shape(int n,int m,int x,int y,int c);
extern void print_next_shape();
extern void eraser_shape(int n,int m,int x,int y);

#endif