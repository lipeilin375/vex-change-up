#include "vex.h"


#if (ROBOT == R_77921F)
const string team_name = "77921F";

motor mtr_LF(PORT11, false);
motor mtr_LB(PORT12, true);
motor mtr_RF(PORT13, true);
motor mtr_RB(PORT14, false);

motor mtr_intake_L(PORT1, ratio18_1, true);
motor mtr_intake_R(PORT2, ratio18_1, false);
motor mtr_carry(PORT3, ratio36_1, false);
motor mtr_shoot(PORT4, ratio36_1, false);

inertial imu(PORT10); 

extern vision::signature vis__BLUE_BALL;
extern vision::signature vis__RED_BALL;
extern vision::signature vis__GREENFLAG;

vision vis = vision(PORT5, 50, vis__GREENFLAG,vis__BLUE_BALL, vis__RED_BALL);
vision vis_front = vision(PORT6, 100, vis__GREENFLAG,vis__BLUE_BALL, vis__RED_BALL);

line carry_entry_detector = vex::line(Brain.ThreeWirePort.E);
line intake_detector = vex::line(Brain.ThreeWirePort.F);

line carry_end_detector = vex::line(Brain.ThreeWirePort.G);
line shoot_detector = vex::line(Brain.ThreeWirePort.H);

#endif