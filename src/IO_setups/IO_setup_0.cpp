#include "vex.h"

#if (ROBOT == R_74000)
const string team_name = "74000";

motor mtr_LF(PORT1, false);
motor mtr_LB(PORT2, true);
motor mtr_RF(PORT3, true);
motor mtr_RB(PORT4, false);

motor mtr_intake_L(PORT16, ratio18_1, false);
motor mtr_intake_R(PORT17, ratio18_1, true);
motor mtr_carry(PORT12, ratio36_1, true);
motor mtr_shoot(PORT11, ratio36_1, false);

/*
左看右看:yaw-z
点头摇头：pitch-y
左摆右摆：roll-x
*/
inertial imu(PORT5);

extern vision::signature vis__BLUE_BALL;
extern vision::signature vis__RED_BALL;
vision vis = vision(PORT20, 50, vis__BLUE_BALL, vis__RED_BALL);

#endif