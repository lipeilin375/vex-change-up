#include "../_includes.h"

void motor_enc_disp() {
  const color clr[6] = {color(ClrViolet),    color(ClrOrangeRed),
                        color(ClrPeachPuff), color::cyan,
                        color::cyan,         color::green};
  int line_y = 50;
  int n_clr = 0;
  screen_set_pen_font(fontType::mono20);
  for (auto iter : ActionUnit::vec_ptr_all_instacnes) {
    if (iter->get_name() == "Chasis_l" || iter->get_name() == "Chasis_r")
      break;
    screen_set_pen_color(clr[n_clr]);
    Print(0, line_y += 18, true, "%-6s enc: %-10d", iter->get_name().c_str(),
          (int)iter->get_enc());
    n_clr++;
  }

  // print dist-mm of chasis
  screen_set_pen_color(color::green);
  static char str[20];
  sprintf(str, "%d mm", (int)chasis.get_dist_mm(-1));
  Print(0, line_y += 18, true, "Chasis  LB: %-13s", str);
  sprintf(str, "%d mm", (int)chasis.get_dist_mm(1));
  Print(0, line_y += 18, true, "Chasis  RB: %-13s", str);

  line_y += 10; 
  screen_set_pen_color(color::cyan);
  Print(0, line_y += 18, true, "seneor   intake: %d - %2d", BALL_AT_INTAKE,
        intake_detector.value(percentUnits::pct));
  Print(0, line_y += 18, true, "seneor    shoot: %d - %2d", BALL_AT_SHOOTER,
        shoot_detector.value(percentUnits::pct));
  Print(0, line_y += 18, true, "sor carry   end: %d - %2d", BALL_AT_CARRIER_END,
        carry_end_detector.value(percentUnits::pct));
  Print(0, line_y += 18, true, "sor carry entry: %d - %2d",
        BALL_AT_CARRIER_ENTRY, carry_entry_detector.value(percentUnits::pct));
}
 
void disp_page_sensors() {
  screen_set_pen_color(color::cyan);
  screen_set_pen_font(fontType::prop20);
  int line_y = 0;
  Print(0, line_y += 20, "         Sensor Data        ");

  line_y += 3;
  screen_set_pen_color(color::yellow);
  screen_set_pen_font(fontType::mono20);
  Print(0, line_y += 18, "imu gyro z: %.2f     ", imu.rotation());

  motor_enc_disp();
}
