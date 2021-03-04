#include "../_includes.h"

bool move_stuck() {
  return (abs(mtr_RB.velocity(velocityUnits::dps)) < 100 &&
          abs(mtr_LB.velocity(velocityUnits::dps)) < 100);
}

void s_move_back_to_wall(float voltage_pct, int dist_fist_direct_back,
                         int dist_fist_turn, int dist_angle_back,
                         int dist_second_turn, int time_stick_to_wall,
                         int side) {
  float k1 = 0.2;
  float k2 = 0.2;
  float k0 = 1;
  timer t;

  mtr_LB.resetRotation();
  t.reset();
  chasis.LR_set_speed(-80, -80);
  while_wait_timeout(
      mtr_LB.rotation(deg) > -dist_fist_direct_back, 1800);

  t.reset();
  if (side == 1) {
    mtr_RB.resetRotation();
    while (mtr_RB.rotation(deg) > -dist_fist_turn && t.time() < 2500) {
      chasis.LR_set_speed(-voltage_pct * k1, -voltage_pct * k0);
    }
  } else if (side == -1) {
    mtr_LB.resetRotation();
    while (mtr_LB.rotation(deg) > -dist_fist_turn && t.time() < 2500) {
      chasis.LR_set_speed(-voltage_pct * k0, -voltage_pct * k1);
    }
  }

  t.reset();
  mtr_LB.resetRotation();
  while (mtr_LB.rotation(rotationUnits::deg) > -dist_angle_back &&
         t.time() < 1800) {
    chasis.LR_set_speed(-voltage_pct, -voltage_pct);
  }

  t.reset();
  if (side == 1) {
    mtr_LB.resetRotation();
    while (mtr_LB.rotation(rotationUnits::deg) > -dist_second_turn &&
           t.time() < 2000) {
      chasis.LR_set_speed(-voltage_pct * k0, -voltage_pct * k2);
    }
  } else if (side == -1) {
    mtr_RB.resetRotation();
    while (mtr_RB.rotation(rotationUnits::deg) > -dist_second_turn &&
           t.time() < 2000) {
      chasis.LR_set_speed(-voltage_pct * k2, -voltage_pct * k0);
    }
  }

  // stick to wall
  t.reset();
  chasis.LR_set_voltage(-40, -40);
  // while (abs(mtr_RB.velocity(velocityUnits::dps)) < 5 &&
  //        abs(mtr_LB.velocity(velocityUnits::dps)) < 5)
  //   ;

  int n_move_stuck = 0;
  while (1) {
    if (move_stuck())
      n_move_stuck++;
    else
      n_move_stuck = 0;

    if (n_move_stuck > 3) {
      // cout << "move stuck ok.." << endl;
      // cout << "stick time:" << t.time() << endl;
      wait(100);
      break;
    }

    wait(10);
    if (t.time() > time_stick_to_wall) {
      // cout << "stick time out" << endl;
      break;
    }
  }
  chasis.FT_set_speed(0, 0);
  chasis.stop();
  wait(10);
}
