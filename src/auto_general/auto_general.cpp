#include "../_includes.h"

void clear_auto_info_line(int32_t x, int32_t y, fontType ft) {
  screen_set_pen_font(ft);
  Print(x, y, "                                                        ");
}
//////////////////////////////////////////////////////////////////////////////////////////////
int intake_n = 3;
int shoot_n = 3;
int intake_cnt = 0;
int shoot_cnt = 0;
int balls_in_robot = 1;
bool shoot_intake_go_on = true;
bool ball_on_center_line_intaken = false;
bool fast_leave = false;
#define c_vtg_on_shooting 80
#define c_vtg_on_wait_for_intake 30

///////////////////////////////////////////////////////////////////////////////////////////////
void auto_ball_status_print() {
  while (autonomous_running) {
    // ball_in_robot = 1 + intake_cnt - shoot_cnt;
    auto_status_print(5, "b:%d  cv:%2d  i:%d  cy:%d  cd:%d  s:%d  ",
                      balls_in_robot, (int)carry.get_voltage_pct(),
                      BALL_AT_INTAKE, BALL_AT_CARRIER_ENTRY,
                      BALL_AT_CARRIER_END, BALL_AT_SHOOTER);
    wait(25);
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////
void shoot_counting_ctrl() {
  shoot_cnt = 0;
  timer t_shoot;
  auto_print_60(0, AutoPrint_shoot_y, "Shoot Start      \n");
  while (shoot_cnt < shoot_n && autonomous_running && shoot_intake_go_on) {
    if (BALL_AT_SHOOTER) {
      auto_print_60(0, AutoPrint_shoot_y, "Shoot %d: %d begin    ", shoot_n,
                    shoot_cnt + 1);
      carry.set_voltage(25); // carry slower when the former begins shooting

      while (BALL_AT_SHOOTER) {
        if (shoot_cnt + 1 == shoot_n && BALL_AT_CARRIER_END) {
          carry.stop();
          shoot_cnt++;
          balls_in_robot--;
          break;
        }
        wait(2);
      }
      t_shoot.reset(); // timing after shooting one ball
      shoot_cnt++;
      balls_in_robot--;
      auto_print_60(0, AutoPrint_shoot_y, "Shoot %d: %d pass    ", shoot_n,
                    shoot_cnt);
    } // find ball

    // carry speed ctrl after shooting one ball
    if (shoot_cnt < shoot_n) {
      if (t_shoot.time() < 450) {
        if (BALL_AT_CARRIER_END)
          carry.stop(); // shooting interval stop
        else {
          if (shoot_cnt > intake_cnt)
            carry.set_voltage(c_vtg_on_wait_for_intake); // s faster than i
          else
            carry.set_voltage(c_vtg_on_shooting); // resume carry
        }
      } else {
        if (shoot_cnt > intake_cnt)
          carry.set_voltage(c_vtg_on_wait_for_intake); // s faster than i
        else
          carry.set_voltage(c_vtg_on_shooting); // resume carry
      }
    } else
      carry.stop(); // shoot done stop

    wait(5);
  }
  auto_print_60(0, AutoPrint_shoot_y, "Shoot %d: %d Done    ", shoot_n,
                shoot_cnt);

  // shooting done
  wait(350); // wait for ball to fall
  shoot.stop();
  if (intake_cnt >= intake_n - 1) // intake last two ball
    carry.stop();                 // stop
  else
    carry.set_voltage(10);           // slow down
  while_wait(intake_cnt < intake_n); // wait for intake thread done

  // clear disp in 500ms
  timer::event(
      [] {
        if (autonomous_running)
          auto_print_60(0, AutoPrint_shoot_y, "                             ");
      },
      500);
}
/////////////////////////////////////////////////////////////////
void intake_counting_ctrl() {
  intake_cnt = 0;
  timer t_pass;
  auto_print_60(0, AutoPrint_intake_y, "Intake Start      \n");

  bool intake_ok = false;

  while (!intake_ok && autonomous_running && shoot_intake_go_on) {
    if (BALL_AT_CARRIER_ENTRY && (intake_cnt < intake_n - 1)) {
      auto_print_60(0, AutoPrint_intake_y, "Intake %d: %d begin    ", intake_n,
                    intake_cnt + 1);

      t_pass.reset();
      while (BALL_AT_CARRIER_ENTRY) {
        wait(1);
      }
      cout << "****************intake time: " << t_pass.time() << endl;
      if (t_pass.time() < 50) {
        cout << "****************intake too short, deleting this one" << endl;
      } else { // normal pass
        intake_cnt++;
        balls_in_robot++;
        if (intake_cnt == intake_n - 1) // intake slower for the last one
          intake.set_voltage(50);
      }

      auto_print_60(0, AutoPrint_intake_y, "Intake %d: %d pass    ", intake_n,
                    intake_cnt);
      wait(150); // wait for the former ball completely carried in
    }

    if (intake_cnt == intake_n - 1 && BALL_AT_INTAKE && !intake_ok) // last one
    {
      intake_cnt++;
      intake_ok = true;
      intake.act_for_enc_relative(-200, 100, 350, false); // last one
    }
    wait(5);
  } // while  !intake_ok

  auto_print_60(0, AutoPrint_intake_y, "Intake %d: %d , Done  ", intake_n,
                intake_cnt);
  // clear disp in 500ms
  timer::event(
      [] {
        if (autonomous_running)
          auto_print_60(0, AutoPrint_intake_y, "                             ");
      },
      500);
}
//////////////////////////////////////////////////////////////////////////////////////////////
void intake_shoot_balls() {
  thread t_i(intake_counting_ctrl);
  thread t_s(shoot_counting_ctrl);
  shoot_intake_go_on = true;
  intake.set_voltage(100);
  carry.set_voltage(100);

  while_wait_timeout(!BALL_AT_INTAKE, 300);
  auto_detail_disp("front ball intaken");
  wait(300);
  chasis.FT_set_voltage(75, 0); // push goal corner
  wait(600);
  auto_detail_disp("shoot start"); 
  shoot.set_voltage(100);
  wait(300); // wait for shoot 1st ball

  timer t;
  int retry = 0;
  bool all_done = false;
  while ((t_i.joinable() || t_s.joinable()) && shoot_intake_go_on &&
         autonomous_running) {
    if (intake_cnt <= intake_n) {
      retry++;
      auto_detail_disp("retry: %d     back off ", retry);
      chasis.forward_dist_relative(-55, 40, 500, true);

      if (fast_leave) // leave once done after back off
        if (shoot_cnt == shoot_n && intake_cnt == intake_n) {
          all_done = true;
          break;
        }

      wait(200);
      if (!shoot_intake_go_on)
        break;
      auto_detail_disp("retry: %d     forward ", retry);
      chasis.forward_dist_relative(55, 40, 500, true);
      chasis.FT_set_voltage(45, 0);
      if (!shoot_intake_go_on)
        break;
      wait(200);
    }
    if (shoot_cnt == shoot_n && intake_cnt == intake_n) {
      all_done = true;
      break;
    }
  }

  if (all_done)
    auto_detail_disp("i_s all done ");
  if (!shoot_intake_go_on)
    auto_detail_disp("i_s force stop ");

  t_i.interrupt();
  t_s.interrupt();
  shoot.stop();
  intake.stop();
  carry.stop();
}
///////////////////////////////////////////////
bool stop_intaken = false;
void ball_on_center_intaken_chk() {
  ball_on_center_line_intaken = false;
  timer t;
  while (t.time() < 2500) {
    if (BALL_AT_INTAKE) {
      ball_on_center_line_intaken = true;
      if (stop_intaken)
      {
        while_wait_timeout(!BALL_AT_CARRIER_ENTRY,500);
        intake.stop();
      }
      break;
    }
    wait(5);
  }
}

///////////////////////////////////////
bool find_shoot_block_ball() {
  shoot.set_voltage(-100);
  timer txx;
  while (txx.time() < 1000) // in case ball block
  {
    if (BALL_AT_SHOOTER) {
      while_wait_timeout(BALL_AT_SHOOTER, 350);
      shoot.stop();
      cout << "!!!!!!!!!!!!!!!!!!!find rev ball\n";
      return true;
    }
    wait(5);
  }
  shoot.stop();
  return false;
}
