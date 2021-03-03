#pragma once
#include "../auto_lib/auto_lib.h"
#include "stdio.h"

#define PRINTF                                                                 \
  if (1)                                                                       \
  printf

//////////////////////////////自动程序运行显示
#define AutoPrint_intake_y 45
#define AutoPrint_shoot_y 120
#define AutoPrint_status_y 175
#define AutoPrint_detail_y 225

void clear_auto_info_line(int32_t x, int32_t y, fontType ft);

template <typename... Arg>
void auto_print_60(int32_t x, int32_t y, Arg... other) {
  clear_auto_info_line(x, y, fontType::prop60);
  screen_set_pen_font(fontType::prop60);
  Print(x, y, true, other...);
  PRINTF(other...);
  PRINTF("\n");
}

template <typename... Arg>
void auto_print_40(int32_t x, int32_t y, Arg... other) {
  clear_auto_info_line(x, y, fontType::prop40);
  screen_set_pen_font(fontType::prop40);
  Print(x, y, true, other...);
  PRINTF(other...);
  PRINTF("\n");
}

template <typename... Arg> void auto_detail_disp(Arg... other) {
  auto_print_40(0, AutoPrint_detail_y, other...);
}

template <typename... Arg> void auto_status_print(int32_t x, Arg... other) {
  // clear_auto_info_line(x, AutoPrint_status_y, fontType::prop40);
  screen_set_pen_font(fontType::prop40);
  Print(x, AutoPrint_status_y, true, other...);
  // PRINTF(other...);
  // PRINTF("\n");
}

//////////////////////////////////////////////////////////////////////////////////////
double get_gyro();
double get_gyro_x();
double get_gyro_y();

void gyro_reset();
void turn_pid_absolute(double aim_final, double max_pwr = 50, int time_out = 0,
                       double forward = 0);
// new turn
void turn_pid_adjust_stop();
bool is_turn_pid_adjust_done();
void turn_pid_adjust_agl_absolute(double aim_agl, double max_pwr = 100,
                                  int timeout = 0,
                                  bool waitForCompletion = true,
                                  double max_adjust_pwr = 10);
// auto routes
void auto_role_left_1();
void auto_role_left_2();
void auto_role_left_3();
void auto_role_right_1();
void auto_role_right_2();
void auto_role_right_3();
void auto_skills();
void auto_test();

#define pause() while (!Controller.ButtonX.pressing())

void shoot_one_ball_carry_not_stop();
void shoot_one_ball_carry_stop();
void shoot_one_ball_simple();

void shoot_one_ball(bool get_next_ball_ready = true, bool stop_shooter = true,
                    bool carry_stop = true);
void outtake_one_ball();
void intake_one_ball();

//////////////////////////////////////////////////////////////////////////////////////
bool is_carry_end_ball_opposite();
extern int intake_n;
extern int shoot_n;
extern int intake_cnt;
extern int shoot_cnt;
extern bool shoot_intake_go_on;
extern int balls_in_robot;
extern bool ball_on_center_line_intaken;
extern bool stop_intaken;
extern bool fast_leave;

void auto_ball_status_print();
void ball_on_center_intaken_chk();
bool find_shoot_block_ball();
void intake_counting_ctrl();
void shoot_counting_ctrl();
void intake_shoot_balls();