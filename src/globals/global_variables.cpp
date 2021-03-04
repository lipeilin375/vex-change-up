#include "globals.h"
//////////////
signature vis__BLUE_BALL = signature (1, -3533, -2577, -3055, 6507, 9647, 8077, 3, 0);
signature vis__RED_BALL = signature (2, 6833, 7971, 7402, -819, -225, -522, 5, 0);
signature vis__GREENFLAG = signature (3, -6183, -5355, -5769, -7149, -6153, -6651, 7.5, 0);
////////////////////////////////////
ActionUnit intake("Intake", mtr_intake_L, mtr_intake_R);
ActionUnit carry("Carry", mtr_carry);
ActionUnit shoot("Shoot", mtr_shoot);

ActionUnit chasis_l("Chasis_l", mtr_LB, mtr_LF);
ActionUnit chasis_r("Chasis_r", mtr_RB, mtr_RF);

HChasis chasis(chasis_l, chasis_r, 260, 0.6);

AutoInfor auto_info(SIDE_RED, ROLE_RIGHT, 3); //自动程序默认路线设置

bool flag_usercontrol_ignore = false; //手动程序里的自动测试标志
