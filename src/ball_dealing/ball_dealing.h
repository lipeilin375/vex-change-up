#pragma once
#include "vex.h"

#define vtg_shoot_idling 20

#define key_shoot_rev Controller.ButtonB.pressing()
#define key_shoot_only_manually Controller.ButtonX.pressing()
#define key_shoot_continously_on_intaking Controller.ButtonL2.pressing()
#define key_shoot_only_one_ball Controller.ButtonL1.pressing()

#define key_collect_in Controller.ButtonR1.pressing()
#define key_collect_out Controller.ButtonR2.pressing()

enum class BallsDealingStates {
  stopped,

  shoot_continously_on_intaking,
  shoot_only_one_ball,

  shoot_only_manually,
  shoot_rev,

  collect_in,
  collect_out,
};

extern BallsDealingStates state;
extern bool ready_to_shoot;

bool is_state(BallsDealingStates sta);
void balls_dealing_handle();
void led_indicate();
bool auto_pick_load();
void auto_stop_carry_at_opp_ball();
void fast_prepare_shooting();
void auto_carry_load_ball();
