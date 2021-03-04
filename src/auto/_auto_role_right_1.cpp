#include "../_includes.h"
#include "auto.h"

//右场, 全投右桶
void auto_role_right_1() {

  // wait(5000);
  auto_print_60(0, AutoPrint_shoot_y, "Shoot Start      ");

  timer t_auto;
  timer t;
  gyro_reset();
  chasis.reset_enc();
  shoot.act_for_enc_relative(250, 100, 2000, false); // unlock hat展开

  chasis.forward_pid_adjust_dist_relative(-220, 100, 2500, true, true, true);

  if (auto_info.role == ROLE_LEFT)////////////////判断左右场
    turn_pid_adjust_agl_absolute(-41.5, 75, 1500, true); // turn to goal left
  if (auto_info.role == ROLE_RIGHT)
    turn_pid_adjust_agl_absolute(-43, 75, 1500, true); // turn to goal right

  shoot.hold_stop();

  // intake ball at front of goal

  intake.set_voltage(100);
  carry.set_voltage(30); // carrry slowly in case stuck确保预装准备好

  intake_n = 3;
  shoot_n = 3;//////////////////////////////////////总计吐球和吸球个数，一般只用改这里！！！！！！！！！！！！！！！！
  thread t_i_corner(intake_counting_ctrl);///////数吸球个数线程开启

  chasis.reset_enc();
  chasis.forward_pid_adjust_dist_relative(630, 100, 1300, false, false,
                                          false); // goto goal right直走接近框
  while_wait_timeout(chasis.get_dist_mm(-1) < 430, 1000);////////////////////走到一半就可以开始射第一个球
  shoot.set_voltage(100);
  carry.set_voltage(100);
  thread t_s_corner(shoot_counting_ctrl);///////数射球个数线程开启

  chasis.FT_set_voltage(60, 0); // push goal hard抵住框

  auto_detail_disp("shooting 1st ball               ");

  while_wait_timeout(shoot_cnt < 1, 500); // wait for stable shoot 1 ball等待稳定射入第一个球，保证占领
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////以下内容基本不动
  int actuall_intake_cnt = 1;
  bool need_to_retry = false;///////是否retry
  if (intake_cnt == 3) {
    auto_detail_disp("So easy, shooting 2 and 3             ");
    while_wait_timeout(shoot_cnt < 3, 1000); // wait for stable shoot 2nd ball
    if (shoot_cnt < 3)
      need_to_retry = true; // goto retry
  } else
    need_to_retry = true; // goto retry

  if (need_to_retry) {
    auto_detail_disp("waiting for intake 2 and 3             ");
    shoot.stop();
    carry.set_voltage(30);
    wait(100);
    t.reset();
    int retry = 0;
    while (retry < 5 && (t_i_corner.joinable() || t_s_corner.joinable())) {
      retry++;
      auto_detail_disp("retry: %d     back off ", retry);

      chasis.forward_dist_relative(-85, 60, 500, true); // back a little
      wait(100);
      auto_detail_disp("retry: %d     forward ", retry);
      chasis.forward_dist_relative(85, 65, 500, true); // hit hard
      chasis.FT_set_voltage(80, 0);                    // push goal hard
      if (intake_cnt == 3)
        break;
      while_wait_timeout(intake_cnt < 3, 450);
    } // retry 2 times
    carry.stop();
    intake.stop();
    chasis.forward_dist_relative(-20, 65, 300, true); // back for shoot off goal

    // retry 2 times over
    if (intake_cnt == 1) {
      auto_detail_disp("corner too tight, 1 left");
      actuall_intake_cnt = 1;
    } else if (intake_cnt == 2) {
      auto_detail_disp("intake 2? try shoot 2");
      carry.set_voltage(100);
      shoot.set_voltage(100);
      // back off in case shoot stuck on goal
      chasis.FT_set_voltage(-30, 0);
      while_wait_timeout(shoot.get_voltage_pct() < 10, 500);
      chasis.stop();
      while_wait_timeout(shoot_cnt < 2, 800); // shoot 2nd  again
      shoot.stop();
      carry.set_voltage(25);
      if (shoot_cnt == 2)
        actuall_intake_cnt = 2;
      else
        actuall_intake_cnt = 1;
      auto_detail_disp("actually intake shot: %d", actuall_intake_cnt);
    } else if (intake_cnt == 3) {
      auto_detail_disp("intake 3? try shoot 2 and 3");
      carry.set_voltage(100);
      shoot.set_voltage(100);
      // back off in case shoot stuck on goal
      chasis.FT_set_voltage(-30, 0);
      while_wait_timeout(shoot.get_voltage_pct() < 10, 500);
      chasis.stop();
      while_wait_timeout(shoot_cnt < 3, 1000); // shoot 2nd and 3rd  again
      if (shoot_cnt == 2)
        actuall_intake_cnt = 2;
      else if (shoot_cnt == 3)
        actuall_intake_cnt = 3;
      else
        actuall_intake_cnt = 1;
      auto_detail_disp("actually intake shot: %d", actuall_intake_cnt);
    }
  } // need to retry

  ///////////////////////////////////////////////////////////////////////////
  // try last chance when leaving
  intake.act_for_enc_relative(-200, 100, 300, false); // in case pull mine out
  wait(100);
  intake.act_for_enc_relative(2500, 100, 1000, false); // intake opp ball in
  carry.act_for_enc_relative(4000, 100, 1000, false);  // intake opp ball in

  chasis.forward_dist_relative(-550, 35, 1000, true); // back off from goal
  if (is_carry_end_ball_opposite()) {
    auto_detail_disp("opp ball!!");
    intake.act_for_enc_relative(-2500, 100, 1000, false);
    carry.act_for_enc_relative(-3500, 100, 1000, true);
  } else
    auto_detail_disp("my ball..");
}