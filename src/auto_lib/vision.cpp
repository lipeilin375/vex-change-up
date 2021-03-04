#include "../_includes.h"

extern vision vis_front;
int c_dx;
void green_backboard_flag_detect() {
  vis_front.setLedColor(255, 255, 255);
  while (true) {
    vis_front.takeSnapshot(vis__GREENFLAG, 2);
    if (vis_front.objectCount > 0) {
      c_dx = vis_front.largestObject.centerX - 160;
      vis_front.setLedColor(0, 255, 0);
    } else {
      vis_front.setLedColor(255, 255, 255);
      c_dx = 0;
    }
    wait(20);
    // chasis.FT_set_voltage(0, c_dx);
  }
  vis_front.setLedColor(0, 0, 0);
}
////////////////////////////////////////////////////////////

/////////////////////////////////////////////
bool is_carry_end_ball_opposite() {
  if (auto_info.side == SIDE_BLUE) {
    vis_front.takeSnapshot(vis__RED_BALL);
    if (vis_front.objectCount > 0)
      return true;
    else
      return false;
  }
  if (auto_info.side == SIDE_RED) {
    vis_front.takeSnapshot(vis__BLUE_BALL);
    if (vis_front.objectCount > 0)
      return true;
    else
      return false;
  }
  return false;
}