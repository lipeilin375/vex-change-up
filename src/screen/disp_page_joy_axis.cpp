#include "../_includes.h"

void disp_page_axis() {
  screen_set_pen_color(color::cyan);
  screen_set_pen_font(fontType::prop20);
  int line_y = 0;
  Print(90, line_y += 20, "Axis Value ");

  const int width = 107, r_point = 2;
  const int x1 = 4, space = 20, x2 = x1 + width + space;
  const int y = 40;
  const int cx1 = x1 + width / 2, cx2 = x2 + width / 2, cy = y - -width / 2;

  // pad
  screen_set_pen_color(color::yellow);
  Brain.Screen.drawRectangle(x1, y, width, width);
  Brain.Screen.drawRectangle(x2, y, width, width);

  // center point
  screen_set_pen_color(color::red);
  Brain.Screen.drawCircle(cx1, cy, r_point);
  Brain.Screen.drawCircle(cx2, cy, r_point);

  screen_set_pen_color(color::yellow);
  // point 1
  int a4 = Controller.Axis4.position(), a3 = Controller.Axis3.position();
  int pcx1 = cx1 + a4 / 2;
  int pcy1 = cy - a3 / 2;
  Brain.Screen.drawCircle(pcx1, pcy1, r_point);

  // point2
  int a1 = Controller.Axis1.position(), a2 = Controller.Axis2.position();
  int pcx2 = cx2 + a1 / 2;
  int pcy2 = cy - a2 / 2;
  Brain.Screen.drawCircle(pcx2, pcy2, r_point);

  screen_set_pen_color(color::green);
  screen_set_pen_font(fontType::mono15);
  line_y = 140;
  Print(x1, line_y += 20, " Axis   X: %-4d", Controller.Axis4.position());
  Print(x1, line_y += 20, " Axis   Y: %-4d", Controller.Axis3.position());
  Print(x1, line_y += 20, " Chasis F: %-4d",
        (int)(chasis_l.get_power_pct() + chasis_r.get_power_pct()) / 2);
  Print(x1, line_y += 20, " Chasis T: %-4d",
        (int)(chasis_l.get_power_pct() - chasis_r.get_power_pct()) / 2);

  line_y = 140;
  Print(x2, line_y += 20, " Axis  X: %-4d", Controller.Axis1.position());
  Print(x2, line_y += 20, " Axis  Y: %-4d", Controller.Axis2.position());
}