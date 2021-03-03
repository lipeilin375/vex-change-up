#include "../_includes.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

void clear_space(int x1, int x2, int y1, int y2) {
  screen_store_pen_color();
  screen_set_pen_color(color::black);
  Brain.Screen.drawRectangle(x1, y1, x2 - x1, y2 - y1, color::black);
  screen_restore_pen_color();
}

int line_y = 0;
int name_y = 90;
int column_x[] = {0,           name_y,       name_y + 30, name_y + 60,
                  name_y + 82, name_y + 122, name_y + 158};

void print_column_title(string title, int colmun_k) {
  int width = 6;
  Print((column_x[colmun_k - 1] + column_x[colmun_k]) / 2 -
            width * title.size() / 2,
        line_y, true, "%s", title.c_str());
}
/////////////////////////////////////////////
void print_motor_data(motor &m, double data, int colmun_k) {
  char str[6];
  int width = 8;
  clear_space(column_x[colmun_k - 1] + 1, column_x[colmun_k] - 1, line_y + 2,
              line_y - 15);

  if (m.installed())
    sprintf(str, "%d", (int)data);
  else
    sprintf(str, "%c", '/');

  Print((column_x[colmun_k - 1] + column_x[colmun_k]) / 2 -
            width * strlen(str) / 2,
        line_y, true, "%s", str);
}
///////////////////////////////////////////////////////////////
void print_motor_info(motor &m, string name) {

  Print(column_x[0] + 5, line_y, true, "%s", name.c_str());

  Print(column_x[1] + 4, line_y, true, "P%-2d", m.index() + 1);
  Print(column_x[2] + 4, line_y, true, "%-3s", m.installed() ? "YES" : "NO");

  print_motor_data(m, m.temperature(temperatureUnits::celsius), 4);
  print_motor_data(m, m.velocity(velocityUnits::rpm), 5);
  print_motor_data(m, m.current(currentUnits::amp) * 1000, 6);

  line_y += 20;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void disp_page_ports() {
  screen_set_pen_color(color(ClrSlateGray));

  line_y = 0;

  screen_set_pen_color(color::cyan);
  screen_set_pen_font(fontType::prop20);

  string title = team_name + "  IO SETUP";
  int x = column_x[6] / 2 -
          title.length() * screen_get_font_width(fontType::prop20) / 2;
  Print(x, line_y += 20, title.c_str());

  screen_set_pen_color(color::white);
  screen_set_pen_font(fontType::mono15);

  line_y += 5;
  screen_set_pen_color(color::yellow);
  Print(5, line_y += 20, "imu: P%d    vis: P%d", imu.index() + 1,
        vis.index() + 1);
  line_y += 10;

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  screen_set_pen_color(color(ClrAquamarine));
  line_y += 10;
  for (int k = 1; k < sizeof(column_x) / sizeof(int); k++)
    Brain.Screen.drawLine(column_x[k], line_y, column_x[k], LCD_MAX_Y);

  screen_set_pen_font(fontType::mono12);
  print_column_title("motor", 1);
  print_column_title("port", 2);
  print_column_title("link", 3);
  print_column_title("temp", 4);
  print_column_title("spd", 5);
  print_column_title("amp", 6);

  screen_set_pen_font(fontType::mono15);
  line_y += 22;

  const color clr[6] = {color(ClrViolet),    color(ClrOrangeRed),
                        color(ClrPeachPuff), color::cyan,
                        color::cyan,         color::green};
  int n_clr = 0;
  static char name_str[20];
  for (auto iter : ActionUnit::vec_ptr_all_instacnes) {
    screen_set_pen_color(clr[n_clr]);
    for (int k = 0; k < iter->get_motor_count(); k++) {
      if (iter->get_motor_count() >= 2) {
        if (iter->get_name() == "Chasis_l" || iter->get_name() == "Chasis_r") {
          char c = (k == 1) ? 'f' : 'b'; // 1=f,0=b
          sprintf(name_str, "%s_%c", iter->get_name().c_str(), c);
        }
        if (iter->get_name() == "Intake") {
          char c = (k == 1) ? 'r' : 'l'; // 1=r,0=l
          sprintf(name_str, "%s_%c", iter->get_name().c_str(), c);
        }
      } else
        sprintf(name_str, "%s", iter->get_name().c_str());
      print_motor_info(*iter->vec_ptr_motors[k], name_str);
    }
    n_clr++;
  }
}
