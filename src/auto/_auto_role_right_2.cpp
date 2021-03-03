#include "../_includes.h"

//不要中线球，角痛和中桶
void auto_role_right_2() {
  auto_print_60(0, AutoPrint_shoot_y, "Shoot Start      ");

  timer t_auto;
  timer t;
  gyro_reset();
  chasis.reset_enc();
  shoot.act_for_enc_relative(250, 100, 2000, false); // unlock hat

  chasis.forward_pid_adjust_dist_relative(-220, 100, 2500, true, true, true);

  if (auto_info.role == ROLE_LEFT)
    turn_pid_adjust_agl_absolute(41.5, 75, 1500, true); // turn to goal left
  if (auto_info.role == ROLE_RIGHT)
    turn_pid_adjust_agl_absolute(-43, 75, 1500, true); // turn to goal right

  shoot.hold_stop();

  // intake ball at front of goal

  intake.set_voltage(100);
  carry.set_voltage(30); // carrry slowly in case stuck

  intake_n = 3;
  shoot_n = 3;
  thread t_i_corner(intake_counting_ctrl);

  chasis.reset_enc();
  chasis.forward_pid_adjust_dist_relative(630, 100, 1300, false, false,
                                          false); // goto goal right
  while_wait_timeout(chasis.get_dist_mm(-1) < 430, 1000);
  shoot.set_voltage(100);
  carry.set_voltage(100);
  thread t_s_corner(shoot_counting_ctrl);

  chasis.FT_set_voltage(60, 0); // push goal hard

  auto_detail_disp("shooting 1st ball               ");

  while_wait_timeout(shoot_cnt < 1, 500); // wait for stable shoot 1 ball

  int balls_to_center = 0;
  int actuall_intake_cnt = 1;
  bool need_to_retry = false;
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
    while (retry < 3 && (t_i_corner.joinable() || t_s_corner.joinable())) {
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

  chasis.forward_dist_relative(-350, 35, 1000, true); // back off from goal

  if (actuall_intake_cnt == 1) { // 2nd  intake ball might get out when leaving
    actuall_intake_cnt = intake_cnt;
    balls_to_center = actuall_intake_cnt; // 1 or 2
  } else if (actuall_intake_cnt == 2) {   // opp ball might get out when leaving
    if (intake_cnt == 3)
      balls_to_center = 1;
  }
  auto_detail_disp("actually intake: %d when leaving", actuall_intake_cnt);

  ///////////////////////////////////////////////////////////////
  // stop just in case
  if (t_s_corner.joinable()) {
    clear_auto_info_line(0, AutoPrint_intake_y, fontType::prop60);
    t_s_corner.interrupt();
  }
  if (t_i_corner.joinable()) {
    clear_auto_info_line(0, AutoPrint_shoot_y, fontType::prop60);
    t_i_corner.interrupt();
  }
  shoot.stop();

  // turn to go to center goal
  if (auto_info.role == ROLE_LEFT)
    turn_pid_adjust_agl_absolute(90, 75, 1500, true);
  if (auto_info.role == ROLE_RIGHT)
    turn_pid_adjust_agl_absolute(-90, 75, 1500, true);

  // my ball or opp ball termination
  if (actuall_intake_cnt < 3) { // intake my ball in
    // intake out a little in case opp ball at intake end
    intake.act_for_enc_relative(-250, 100, 300, true);
    intake.act_for_enc_relative(1000, 100, 1000, false);
    carry.act_for_enc_relative(1000, 100, 1000, false);
    auto_detail_disp("shoot:%d corner,  %d to center     ", shoot_cnt,
                       balls_to_center);
  } else { // get rid of opp ball
    auto_detail_disp("intake 3, maybe opp ball         ");
    if (is_carry_end_ball_opposite()) {
      intake.act_for_enc_relative(-2500, 100, 1000, false);
      carry.act_for_enc_relative(-3500, 100, 1000, false);
      auto_detail_disp("get rid of opp ball         ");
    } else
      auto_detail_disp("no opp ball          ");
  }

  chasis.forward_pid_adjust_dist_relative(-1180, 100, 2500, true, true, true);

  carry.stop();
  intake.stop();

  // turn to go to center goal
  if (auto_info.role == ROLE_LEFT)
    turn_pid_adjust_agl_absolute(0, 75, 1500, true);
  if (auto_info.role == ROLE_RIGHT)
    turn_pid_adjust_agl_absolute(0, 75, 1500, true);

  /////////////////////////////////////////
  shoot_n = balls_to_center + 1;
  intake_n = 2;
  auto_detail_disp("shoot center in all: %d         ", shoot_n);

  intake.set_voltage(100);
  carry.set_voltage(100);
  thread t_i_center(intake_counting_ctrl);
  chasis.forward_dist_relative(230, 35, 500, true);
  chasis.FT_set_voltage(60, 0); // push goal hard
  wait(200);
  /////////////////////////////////////////////////////////////////
  shoot.set_voltage(100);
  thread t_s_center(shoot_counting_ctrl);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // center goal
  while_wait_timeout(shoot_cnt < 1, 1500);
  auto_detail_disp("shooting 1st ball       ");

  // retry intake when shooting 2 even 3 from corner
  int retry = 0;
  while (retry < 2 && (t_i_center.joinable() || t_s_center.joinable())) {
    if (intake_cnt >= 2)
      break;
    if (t_auto.time() > 14000) {
      cout << "no more time for intake again...\n";
      break;
    }
    retry++;

    auto_detail_disp("retry : %d     back off ", retry);
    chasis.forward_dist_relative(-85, 25, 500, true);
    wait(200);

    if (intake_cnt >= 2)
      break;
    auto_detail_disp("retry: %d     forward ", retry);
    chasis.forward_dist_relative(85, 25, 500, true);

    if (intake_cnt >= 2)
      break;
    chasis.FT_set_voltage(80, 0); // push goal hard
    wait(200);
  }
  if (retry == 2)
    auto_detail_disp("center too tight, give up.....   ");

  // intake reverse prevent my ball pulled out
  intake.act_for_enc_relative(-200, 100, 300, false);
  if (t_auto.time() < 14700) {
    auto_detail_disp("almost auto timeup!!!  ");
    wait(100);
  }

  // stop just in case
  if (t_s_center.joinable())
    t_s_center.interrupt();
  if (t_i_center.joinable())
    t_i_center.interrupt();

  chasis.reset_enc();
  chasis.forward_dist_relative(-450, 75, 1000, false);
  wait(300);

  while (chasis.get_dist_mm(-1) > -300)
    wait(5);

  carry.set_voltage(-100);
  intake.set_voltage(-65);
  wait(500);
  if (t_auto.time() < 14995)
    auto_detail_disp("we made it !!!  ");

  wait(50);
  carry.stop();
  intake.stop();
  //////////////////////
}