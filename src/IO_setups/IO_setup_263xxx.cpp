#include "vex.h"

#if (ROBOT == R_263)
const string team_name = "263";

// move
motor mtr_LF(PORT6, false);
motor mtr_LB(PORT7, true);
motor mtr_RF(PORT8, true);
motor mtr_RB(PORT9, false);

motor mtr_intake_L(PORT3, ratio18_1, false);
motor mtr_intake_R(PORT2, ratio18_1, true);
motor mtr_carry(PORT10, ratio36_1, false);
motor mtr_shoot(PORT1, ratio36_1, false);

inertial imu(PORT20);

extern vision::signature vis__BLUE_BALL;
extern vision::signature vis__RED_BALL;
extern vision::signature vis__GREENFLAG;
vision vis = vision(PORT4, 50, vis__GREENFLAG, vis__BLUE_BALL, vis__RED_BALL);

line shoot_detector = vex::line(Brain.ThreeWirePort.C);
line carry_end_detector = vex::line(Brain.ThreeWirePort.D);
line carry_entry_detector = vex::line(Brain.ThreeWirePort.E);
line intake_detector = vex::line(Brain.ThreeWirePort.F);

#endif
