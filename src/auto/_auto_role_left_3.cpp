#include "../_includes.h"
bool FAST_GET_BALL_ON_LINE = false;

//左场，抢中线球，角和中桶
void auto_role_left_3() {
  cout << "\n\n-------------------------------------------------------------\n";
  thread t_status(auto_ball_status_print);
  timer t_auto;
  timer t;

  gyro_reset();
  chasis.reset_enc();
  shoot.act_for_enc_relative(250, 100, 2000, false); // unlock hat

  if (FAST_GET_BALL_ON_LINE) {
    // fast, may cross line，about 1200ms
    chasis.forward_dist_relative(1100, 100, 3000, false);
  } else {
    // slow but steady, about 2000ms
    chasis.forward_pid_adjust_dist_relative(1220, 100, 3000, false);
  }

  wait(300); // wait for hat unlock
  intake.set_voltage(100);
  carry.set_voltage(100);
  wait(250); // wait for ball carried up
  
  thread ball_c_chk(ball_on_center_intaken_chk);

  if (FAST_GET_BALL_ON_LINE) {
    while (chasis.is_moving()) {
      if (ball_on_center_line_intaken && chasis.get_dist_mm(-1) > 1000) {
        auto_detail_disp("line ball intaken in advance");
        chasis.stop();
      }
      wait(5);
    }
  } else {
    while_wait(!chasis.is_forward_pid_adjust_done());
  }

  cout << "xxxxxxx time: " << t_auto.time() << endl;
  // back from auto line
  if (FAST_GET_BALL_ON_LINE)
    chasis.forward_pid_adjust_dist_relative(-700, 100, 1000, true, true, false);
  else
    chasis.forward_pid_adjust_dist_relative(-750, 100, 1000, true, true, false);

  if (ball_on_center_line_intaken) {
    balls_in_robot++;
    auto_detail_disp("line ball intaken");
  } else
    auto_detail_disp("line ball lost");

  shoot.act_for_enc_relative(-350, 80, 200, false); // ball lower
  carry.act_for_enc_relative(-100, 20, 200, false); // ball lower

  if (auto_info.role == ROLE_LEFT)
    turn_pid_adjust_agl_absolute(130.5, 75, 900, true); // turn to goal left
  if (auto_info.role == ROLE_RIGHT)
    turn_pid_adjust_agl_absolute(135, 75, 900, true); // turn to goal right
  shoot.hold_stop();

  chasis.reset_enc();
  chasis.forward_pid_adjust_dist_relative(800, 100, 2500, false, false,
                                          false); // goto goal right

  while_wait(chasis.get_dist_mm(-1) < 400);
  intake_n = 3;
  shoot_n = 3;
  auto_detail_disp("intake_shoot_balls start ");

  fast_leave = false;
  thread t_i_s_corner(intake_shoot_balls);
  while_wait_timeout(!BALL_AT_INTAKE && !BALL_AT_CARRIER_ENTRY, 500);
  auto_detail_disp("front goal ball intaken");

  chasis.stop();
  chasis.FT_set_voltage(60, 0);
  while (t_i_s_corner.joinable()) {
    if (t_auto.time() > 9000)
      shoot_intake_go_on = false; // force stop
    wait(100);
  }
  // about 7s
  cout << "...corner time:" << t_auto.time() << endl;

  chasis.forward_dist_relative(-400, 35, 1000, false); // back off from goal
  intake.act_for_enc_relative(-300, 100, 250, false);  // in case my ball out
  wait(300); // wait for robot away from goal
  while_wait(chasis.is_moving());
  //////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////
  int balls_to_center = intake_cnt - shoot_cnt;
  if (balls_to_center < 0) // in case error
    balls_to_center = 0;
  // turn to go to center goal
  if (auto_info.role == ROLE_LEFT)
    turn_pid_adjust_agl_absolute(90, 75, 600, true);
  if (auto_info.role == ROLE_RIGHT)
    turn_pid_adjust_agl_absolute(90, 75, 600, true);

  if (intake_cnt == intake_n) {
    if (ball_on_center_line_intaken) {
      auto_detail_disp("get rid of opp ball,my ball up ");
      intake.set_voltage(-100);
      carry.act_for_enc_relative(-500, 25, 400, false);
    } else {
      auto_detail_disp("get rid of only opp ball ");
      intake.set_voltage(-100);
      carry.set_voltage(-100);
    }
  }
  chasis.forward_pid_adjust_dist_relative(-1100, 100, 2500, false); // to center
  bool ball_on_shooter_block = find_shoot_block_ball(); // deal with ball block
  while_wait(!chasis.is_forward_pid_adjust_done());     // wait for

  carry.set_voltage(100); // carry my ball up
  intake.stop();

  // turn to go to center goal
  if (auto_info.role == ROLE_LEFT)
    turn_pid_adjust_agl_absolute(-180, 75, 800, true);
  if (auto_info.role == ROLE_RIGHT)
    turn_pid_adjust_agl_absolute(-180, 75, 800, true);

  /////////////////////////////////////////
  if (ball_on_center_line_intaken)
    shoot_n = balls_to_center + 2;
  else
    shoot_n = balls_to_center + 1;
  if (ball_on_shooter_block)
    shoot_n++;

  intake_n = 2;
  auto_detail_disp("shoot center in all: %d         ", shoot_n);

  chasis.reset_enc();
  chasis.forward_dist_relative(300, 75, 700, true);
  chasis.FT_set_voltage(40, 0); // push goal center

  fast_leave = true;
  thread t_is_center(intake_shoot_balls);

  while (t_is_center.joinable()) {
    wait(100);
    if (t_auto.time() > 14500)
      shoot_intake_go_on = false; // force stop
  }
  // about 14.2s
  cout << "...center time:" << t_auto.time() << endl;

  chasis.reset_enc();
  chasis.forward_dist_relative(-450, 100, 2500, false); // back off from goal
  intake.act_for_enc_relative(-300, 100, 250, false);   // in case my ball out

  while_wait(chasis.get_dist_mm(-1) > -100); // wait for robot away from goal
  if (intake_cnt == intake_n) {
    auto_detail_disp("get rid of opp ball ");
    intake.act_for_enc_relative(-2000, 50, 1000, false);
    carry.act_for_enc_relative(-1000, 50, 1000, false);
  }

  t_status.interrupt();
  if (t_auto.time() < 14995)
    cout << "we made it!!!  time: " << t_auto.time() << endl;

  while_wait(chasis.is_moving() || intake.is_acting_enc());
  cout << "actual auto time: " << t_auto.time() << endl;
  ActionUnit::all_units_stop();
  screen_clear();
}