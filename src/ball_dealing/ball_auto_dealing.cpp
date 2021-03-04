#include "../auto/auto.h"
#include "../globals/globals.h"
#include "ball_dealing.h"

void led_indicate() {
  bool blue = false, red = false;
  static int cnt = 0;

  cnt++;
  cnt %= 10;
  vis.takeSnapshot(vis__BLUE_BALL);
  if (vis.objectCount > 0) {
    vis.setLedColor(0, 0, 255);
    blue = true;
  }

  vis.takeSnapshot(vis__RED_BALL);
  if (vis.objectCount > 0) {
    vis.setLedColor(255, 0, 0);
    red = true;
  }

  if (!blue && !red) { // flashing
    if (cnt > 5) {
      if (auto_info.side == SIDE_RED)
        vis.setLedColor(255, 0, 0);
      if (auto_info.side == SIDE_BLUE)
        vis.setLedColor(0, 0, 255);
    } else
      vis.setLedColor(0, 0, 0);
  } else if (red)
    vis.setLedColor(255, 0, 0);
  else if (blue)
    vis.setLedColor(0, 0, 255);
}

bool auto_pick_load() {
  bool shoot_one_opp_ball = false;
  if (auto_info.side == SIDE_RED) {
    vis.takeSnapshot(vis__BLUE_BALL);
    if (vis.objectCount > 0) {
      vis.setLedColor(0, 0, 255);
      shoot_one_ball(true, true, true);
      shoot_one_opp_ball = true; // opp ball
    }
  }
  if (auto_info.side == SIDE_BLUE) {
    vis.takeSnapshot(vis__RED_BALL);
    if (vis.objectCount > 0) {
      vis.setLedColor(255, 0, 0);
      shoot_one_ball(true, true, true);
      shoot_one_opp_ball = true; // opp ball
    }
  }
  return shoot_one_opp_ball;
}

//////////////////////////////////////////////////////////////
void auto_stop_carry_at_opp_ball() {
  bool blue = false, red = false;

  vis.takeSnapshot(vis__BLUE_BALL);
  if (vis.objectCount > 0) {
    vis.setLedColor(0, 0, 255);
    blue = true;
  }

  vis.takeSnapshot(vis__RED_BALL);
  if (vis.objectCount > 0) {
    vis.setLedColor(255, 0, 0);
    red = true;
  }

  if (auto_info.side == SIDE_BLUE && red)
    carry.stop();
  if (auto_info.side == SIDE_RED && blue)
    carry.stop();
  if (!blue && !red)
    vis.setLedColor(255, 255, 255);
}
//////////////////////////////////////////////////////////////
void fast_prepare_shooting() {
  carry.set_voltage(-100);
  shoot.set_voltage(-100);
  wait(50);
  shoot.set_voltage(100);
  carry.stop();
  ready_to_shoot = true;
}