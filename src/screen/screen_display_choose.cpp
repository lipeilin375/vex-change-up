#include "../globals/globals.h"
#include "screen.h"

// button intances
/*
bv=auto_info.side + auto_info.role
#define SIDE_RED  -1
#define SIDE_BLUE  1
#define ROLE_LEFT   1
#define ROLE_RIGHT   2
*/
const int BV_MAP[4][2] = {{SIDE_RED, ROLE_LEFT},
                          {SIDE_RED, ROLE_RIGHT},
                          {SIDE_BLUE, ROLE_LEFT},
                          {SIDE_BLUE, ROLE_RIGHT}};

BtnRectangle *auto_side_role_btn[4];
BtnRectangle *vb_route;
BtnRectangle *vb_auto;
BtnRectangle *vb_gyro_rec;

int cur_bv_highting;
/////////////////////////////////////////////////
void update_auto_info_display() {
  int line_x = 265;
  int line_y = 65;

  screen_set_pen_color(color::white);
  screen_set_pen_font(fontType::mono20);

  Brain.Screen.printAt(line_x, line_y, "Auto : ");

  if (auto_info.side == SIDE_RED)
    screen_set_pen_color(color::red);
  else
    screen_set_pen_color(color::blue);

  if (auto_info.side == SIDE_RED)
    Brain.Screen.printAt(line_x + 7 * LETTER_WIDTH, line_y, "RED  ");
  else
    Brain.Screen.printAt(line_x + 7 * LETTER_WIDTH, line_y, "BLUE ");

  if (auto_info.role == ROLE_LEFT)
    Brain.Screen.printAt(line_x + 12 * LETTER_WIDTH, line_y, "LEFT  ");
  else
    Brain.Screen.printAt(line_x + 12 * LETTER_WIDTH, line_y, "RIGHT ");

  Brain.Screen.printAt(line_x + 17 * LETTER_WIDTH, line_y, " %d",
                       auto_info.route_number);
}
////////////////////////////////////////////////////////////////////////////////
void route_number_roll_over() {
  if (auto_info.role == ROLE_LEFT &&
      auto_info.route_number > ROLE_LEFT_MAX_AUTO_NUMBER)
    auto_info.route_number = 1;
  if (auto_info.role == ROLE_RIGHT &&
      auto_info.route_number > ROLE_RIGHT_MAX_AUTO_NUMBER)
    auto_info.route_number = 1;
}
////////////////////////////////////////////////////////////////////////////////
void route_number_button_press_callback(BtnShape *p_this_btn) {
  p_this_btn->brighter();
  auto_info.route_number++;
  route_number_roll_over();
  update_auto_info_display();
}
void route_number_button_release_callback(BtnShape *p_this_btn) {
  p_this_btn->darker();
}

////////////////////////////////////////////////////////////////////////////////
void auto_side_role_button_press_callback(BtnShape *p_this_btn) {
  //加上bv值判断，防止同一按键按下会继续执行选场代码
  if (cur_bv_highting != p_this_btn->get_id()) {
    auto_info.side = BV_MAP[p_this_btn->get_id()][0]; //更新选场信息
    auto_info.role = BV_MAP[p_this_btn->get_id()][1];

    auto_side_role_btn[cur_bv_highting]->darker(); //当前高亮的按钮变暗
    cur_bv_highting = auto_info.side + auto_info.role; //更新cur_bv_highting
    p_this_btn->brighter();
    route_number_roll_over();
    update_auto_info_display();
    if (auto_info.side == SIDE_RED)
      vb_route->set_color_fill(red);
    else
      vb_route->set_color_fill(blue);
  }
}
/////////////////////////////////////////
void buttons_init() {
  int width = 108, height = 45, space_x = 8;
  int red_x = 252, blue_x = red_x + width + space_x;
  int y_1 = 120, y_2 = y_1 + height + 15;

  cur_bv_highting = auto_info.side + auto_info.role;

  // bv=0
  static BtnRectangle vb_red1(red_x, y_1, width, height, cur_bv_highting == 0,
                              color::white, color::red, "RED LEFT"); // bv:0
  auto_side_role_btn[0] = &vb_red1;

  // bv=1
  static BtnRectangle vb_red2(red_x, y_2, width, height, cur_bv_highting == 1,
                              color::white, color::red, "RED RIGHT"); // bv:1
  auto_side_role_btn[1] = &vb_red2;

  // bv=2
  static BtnRectangle vb_blue1(blue_x, y_1, width, height, cur_bv_highting == 2,
                               color::white, color::blue,
                               "BLUE LEFT"); // bv:2
  auto_side_role_btn[2] = &vb_blue1;

  // bv=3
  static BtnRectangle vb_blue2(blue_x, y_2, width, height, cur_bv_highting == 3,
                               color::white, color::blue,
                               "BLUE RIGHT"); // bv:3
  auto_side_role_btn[3] = &vb_blue2;

  for (int k = 0; k < 4; k++) {
    auto_side_role_btn[k]->set_id(k);
    auto_side_role_btn[k]->register_press_callback(
        auto_side_role_button_press_callback);
  }

  static BtnRectangle vb_route_tmp(
      red_x, y_1 - height - 2, width * 2 + space_x, height - 10, false,
      color::white, color::white, "ROUTE", fontType::mono20, cyan);
  vb_route = &vb_route_tmp;
  vb_route->register_press_callback(route_number_button_press_callback);
  vb_route->register_release_callback(route_number_button_release_callback);

  //////////////////////////////
  static BtnRectangle btn_gyro_racallibrate(
      red_x, 10, width, 35, false, color::white, color::orange, "imu calibrate",
      fontType::mono15, color::white);
  static BtnRectangle btn_auto_start(blue_x, 10, width, 35, false, color::white,
                                     color::orange, "start auto",
                                     fontType::mono15, color::white);
  vb_auto = &btn_auto_start;
  vb_gyro_rec = &btn_gyro_racallibrate;
  update_auto_info_display();
}
////////////////////////////////////////////////////////////////
void buttons_show_up() {
  for (auto b : auto_side_role_btn)
    b->show_up();
  vb_route->show_up();
  vb_auto->show_up();
  vb_gyro_rec->show_up();
  update_auto_info_display();
}