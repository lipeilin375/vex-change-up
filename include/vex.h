#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iostream"
#include "robot-config.h" 
#include "v5.h"
#include "v5_vcs.h"
using namespace vex;
using namespace std;

extern const string team_name; // defined in IO_setup_xxx.cpp
#define R_74000 0
#define R_74000S 1
#define R_74000V 2
#define R_74000W 3
#define R_77921F 4
#define R_263 5 
#define R_20505 6
#define R_7792 7

#define ROBOT R_77921F

// gloabl
extern vex::brain Brain;
extern vex::controller Controller;
extern vex::competition Competition;

extern color screen_brainPenColor; //全局画笔颜色
void screen_set_pen_color(color c);
color screen_get_pen_color();
void screen_restore_pen_color(); //保存画笔颜色
void screen_store_pen_color();   //恢复画笔颜色

#define Print Brain.Screen.printAt

void wait(int milsec);

#define while_wait(condition)                                                  \
  do {                                                                         \
    wait(5);                                                                   \
  } while (condition)

#define while_wait_timeout(condition, timeout)                                 \
  {                                                                            \
    timer _t;                                                                  \
    do {                                                                       \
      wait(5);                                                                 \
    } while (condition && _t.time() < timeout);                                \
  }

#define repeat(count) for (int n = 0; n < count; n++)

#define waitUntil(condition)                                                   \
  do {                                                                         \
    wait(5, msec);                                                             \
  } while (!(condition))

void pre_auton(void);
void _autonomous(void);
void _usercontrol(void);

#define abs(x) (((x) > 0) ? (x) : -(x))
int sgn(double x);
void zero_out_blind(double &x, double threshold);
void max_limit(double &x, double max_bnd);
void min_limit(double &x, double min_bnd);

#define Critical_Error_Null_Ptr(info)
