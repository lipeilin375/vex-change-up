#pragma once
#include "../Hchasis/Hchasis.h"
#include "../ball_dealing/ball_dealing.h"
///////////////////////////////////////////////////////////////
using signature = vision::signature;
using code = vision::code;

// VEXcode devices
extern signature vis__BLUE_BALL;
extern signature vis__RED_BALL;
extern signature vis__GREENFLAG;
extern signature vis__SIG_4;
extern signature vis__SIG_5;
extern signature vis__SIG_6;
extern signature vis__SIG_7;
extern vision vis;

////////////////////////////////////////////////////////////////////////
/*
马达和传感器
*/
extern vex::motor mtr_LF;
extern vex::motor mtr_LB;
extern vex::motor mtr_RF;
extern vex::motor mtr_RB;

extern vex::motor mtr_intake_L;
extern vex::motor mtr_intake_R;
extern vex::motor mtr_carry;
extern vex::motor mtr_shoot;

extern vex::inertial imu;
extern line carry_entry_detector;
extern line intake_detector;
extern line carry_end_detector;
extern line shoot_detector;

/////////////////////////////////////////////////////////////////////
/*
动作单元
*/
extern ActionUnit intake;
extern ActionUnit carry;
extern ActionUnit shoot;

extern ActionUnit chasis_l;
extern ActionUnit chasis_r;
extern HChasis chasis;
////////////////////////////////////////////////////////////////////////
/*
自动程序路线信息
*/
//不要修改！！！
#define SIDE_RED -1
#define SIDE_BLUE 1

#define ROLE_LEFT  1
#define ROLE_RIGHT 2

#define ROLE_LEFT_MAX_AUTO_NUMBER 4
#define ROLE_RIGHT_MAX_AUTO_NUMBER 4

class AutoInfor {
public:
  int side;         ///红蓝
  int role;         /// 1还是2区
  int route_number; ///线路号
  AutoInfor(int s, int r, int n) : side(s), role(r), route_number(n) {}
};

extern AutoInfor auto_info;
////////////////////////////////////////////////////////////////////////
extern thread thread_autonomous;
extern bool autonomous_running;
////////////////////////////////////////////////////////////////////////
