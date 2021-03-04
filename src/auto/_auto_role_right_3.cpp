#include "../_includes.h"
bool FAST_GET_BALL_ON__THE_LINE = false;

//左场，抢中线球，角痛和中桶
void auto_role_right_3() {

  cout << "xxxxxx\n";

  cout << "\n\n-------------------------------------------------------------\n";
  thread t_status(auto_ball_status_print);
  timer t_auto;
  timer t;

  gyro_reset();
  chasis.reset_enc();
  shoot.act_for_enc_relative(250, 100, 2000, false); // unlock hat

  if (FAST_GET_BALL_ON__THE_LINE) {
    // fast, may cross line，about 1200ms
    chasis.forward_dist_relative(1100, 100, 3000, false);
  } else {
    // slow but steady, about 2000ms
    chasis.forward_pid_adjust_dist_relative(1220, 100, 3000, false);
  }

  wait(300); // wait for hat unlock
  intake.set_voltage(100);
  // carry.set_voltage(100);
  wait(250); // wait for ball carried up
  stop_intaken = true;
  thread ball_c_chk(ball_on_center_intaken_chk);
  if (FAST_GET_BALL_ON__THE_LINE) {
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
  if (FAST_GET_BALL_ON__THE_LINE)
    chasis.forward_pid_adjust_dist_relative(-770, 100, 1000, true, true, false);
  else
    chasis.forward_pid_adjust_dist_relative(-750, 100, 1000, true, true, false);

  if (ball_on_center_line_intaken) {
    balls_in_robot++;
    auto_detail_disp("line ball intaken");
    turn_pid_adjust_agl_absolute(85, 75, 900, true); // turn to goal right
    carry.act_for_enc_relative(-200, 100, 500, false);
    intake.act_for_enc_relative(-2000, 50, 1000, true);
    carry.act_for_enc_relative(1000, 100, 1000, false);
    wait(500);
  } else
    auto_detail_disp("line ball lost");

  shoot.act_for_enc_relative(-350, 80, 200, false); // ball lower
  carry.act_for_enc_relative(-100, 20, 200, false); // ball lower

  turn_pid_adjust_agl_absolute(135, 75, 900, true); // turn to goal right
  shoot.hold_stop();

  chasis.reset_enc();
  chasis.forward_pid_adjust_dist_relative(850, 100, 2500, false, false,
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

  t_status.interrupt();
  if (t_auto.time() < 14995)
    cout << "we made it!!!  time: " << t_auto.time() << endl;

  while_wait(chasis.is_moving() || intake.is_acting_enc());
  cout << "actual auto time: " << t_auto.time() << endl;
  ActionUnit::all_units_stop();
  screen_clear();
}