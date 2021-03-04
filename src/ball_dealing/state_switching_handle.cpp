#include "../auto/auto.h"
#include "../globals/globals.h"
#include "ball_dealing.h"

// #define key_separate_in Controller.ButtonA.pressing()

BallsDealingStates state = BallsDealingStates::stopped;

bool is_state(BallsDealingStates sta) { return state == sta; }

bool ready_to_shoot = false;

timer t_shoot_idling;
timer t_going_to_prepare_shoot;

void state_output() {
  // cout<<"carry end ball:"<<BALL_AT_CARRIER_END<<endl;
  // cout<<"shooter   ball:"<<BALL_AT_SHOOTER<<endl<<endl;

  led_indicate();

  ///////////////////////////////////////////////////////////
  if (is_state(BallsDealingStates::stopped)) {
    if (!ActionUnit::is_all_units_relaxing()) { // working
      intake.stop();
      carry.stop();

      if (t_going_to_prepare_shoot.time() > 250) {
        if (!ready_to_shoot) { // prepare shooting
          // pick inadvance
          bool opp_ball_at_top = auto_pick_load();
          if (!opp_ball_at_top) {
            carry.set_voltage(-100);
            shoot.set_voltage(-100);
            if (BALL_AT_SHOOTER) {
              while_wait_timeout(BALL_AT_SHOOTER, 250);
              while_wait_timeout(!BALL_AT_CARRIER_END, 250);
            } else if (BALL_AT_CARRIER_END) {
              wait(50);
            } else
              wait(100);
            // wait(0);
            shoot.set_voltage(vtg_shoot_idling);
            carry.stop();
            ready_to_shoot = true;
          } else {
            shoot.set_voltage(vtg_shoot_idling);
            carry.stop();
            ready_to_shoot = true;
          }

          t_shoot_idling.reset();
          if (!BALL_AT_CARRIER_END) {
            auto_carry_load_ball();
          }
        } else {
          if (t_shoot_idling.time() > 350 && shoot.get_velocity() < 10 &&
              !Controller.ButtonLeft.pressing()) {
            ready_to_shoot = false;
          }
          if (!Controller.ButtonLeft.pressing()) {
            shoot.set_voltage(vtg_shoot_idling);
            auto_pick_load(); //自动筛选球
          }
        }
      }
    } else { // relaxing
      intake.stop();
      carry.stop();
      shoot.stop();
      ready_to_shoot = false;
    }
  }

  if (is_state(BallsDealingStates::shoot_only_manually)) {
  shoot:
    shoot.set_voltage(100);
    carry.set_voltage(45);
    intake.stop();
    while_wait(key_shoot_only_manually);
    shoot.stop();
    carry.stop();
    timer t;
    while (t.time() < 500) {
      if (key_shoot_only_manually)
        goto shoot;
      if (key_collect_in)
        break;
    }
    ready_to_shoot = false;
  }

  if (is_state(BallsDealingStates::shoot_only_one_ball)) {
    if (!ready_to_shoot)
      fast_prepare_shooting();
    else {
      thread t1(shoot_one_ball_simple);
      while (key_shoot_only_one_ball) {
        wait(10);
      }
      while_wait_timeout(t1.joinable(), 300);
      shoot.set_voltage(vtg_shoot_idling);
      ready_to_shoot = true;
    }
  }

  if (is_state(BallsDealingStates::shoot_continously_on_intaking)) {
    if (!ready_to_shoot)
      fast_prepare_shooting();
    else {
      shoot.set_voltage(100);
      intake.set_voltage(100);
      carry.set_voltage(100);

      while (key_shoot_continously_on_intaking) {
        auto_stop_carry_at_opp_ball();
        if (BALL_AT_SHOOTER)
          carry.set_voltage(20);
        else
          carry.set_voltage(100);
        wait(2);
      }

      shoot.set_voltage(vtg_shoot_idling);
      carry.set_voltage(35);
      while_wait_timeout(!BALL_AT_CARRIER_END && !BALL_AT_SHOOTER, 400);
      intake.stop();
      carry.stop();
      ready_to_shoot = true;
    }
  }

  if (is_state(BallsDealingStates::shoot_rev)) {
    ready_to_shoot = false;
    carry.set_voltage(-100);
    shoot.set_voltage(-100);
    intake.act_for_enc_relative(-1200, 100, 1000, true);
    intake.act_for_enc_relative(1500, 100, 800, false);
    carry.act_for_enc_relative(1600, 100, 700, true);
  }

  if (is_state(BallsDealingStates::collect_in)) {
    ready_to_shoot = false;
    intake.set_voltage(100);
    carry.set_voltage(100);
    shoot.stop(brakeType::hold);
    t_going_to_prepare_shoot.reset();
  }

  if (is_state(BallsDealingStates::collect_out)) {
    ready_to_shoot = false;
    if (Controller.Axis3.position() < -85) { // fast back--from center goal
      intake.set_voltage(-25);
      carry.stop();
      shoot.stop();
    } else {
      intake.set_voltage(-100);
      carry.set_voltage(-100);
      shoot.set_voltage(-100);
    }
  }
}

void state_switching_handle() {
  if (key_shoot_continously_on_intaking)
    state = BallsDealingStates::shoot_continously_on_intaking;

  if (key_shoot_rev && !Controller.ButtonLeft.pressing())
    state = BallsDealingStates::shoot_rev;

  if (key_shoot_only_manually && !Controller.ButtonLeft.pressing())
    state = BallsDealingStates::shoot_only_manually;

  if (key_shoot_only_one_ball)
    state = BallsDealingStates::shoot_only_one_ball;

  if (key_collect_in && !key_collect_out)
    state = BallsDealingStates::collect_in;
  else if (key_collect_out && !key_collect_in)
    state = BallsDealingStates::collect_out;

  if (!key_shoot_rev && !key_shoot_only_manually &&
      !key_shoot_continously_on_intaking && !key_collect_in &&
      !key_collect_out && !key_shoot_only_one_ball)
    state = BallsDealingStates::stopped;

  ActionUnit::all_units_stop_on_working(
      !Controller.ButtonLeft.pressing() &&
      (key_shoot_continously_on_intaking || key_shoot_rev ||
       key_shoot_only_manually || key_collect_in || key_collect_out ||
       key_shoot_only_one_ball));
}

#include "../auto_lib/auto_lib.h"

void balls_dealing_handle() {
  while (true) {
    if (!autonomous_running) {
      state_switching_handle();
      state_output();

      if (Controller.ButtonY.pressing()) {
        intake.set_voltage(-100);
      }

      if (Controller.ButtonA.pressing()) {

        intake.set_voltage(100);
        thread t1(shoot_one_ball_carry_not_stop);

        // bool one_ball_at_carry_entrance = BALL_AT_CARRIER_ENTRY;
        while_wait_timeout(BALL_AT_CARRIER_ENTRY, 400);
        // cout << "entrance ball gone\n";

        int cnt_opp_ball = 0;
        while (Controller.ButtonA.pressing()) {
          if (BALL_AT_CARRIER_ENTRY) {
            cnt_opp_ball = 1;
            carry.hold_stop();
            // cout << "one opp ball，carry stop\n";
          }
          // if (cnt_opp_ball == 1 && !BALL_AT_CARRIER_ENTRY) {
          //   ball_1_pushed_up = true;
          //   wait(100);
          // }
          // if (ball_1_pushed_up && BALL_AT_CARRIER_ENTRY)
          //   cnt_opp_ball = 2;
          wait(5);
        }

        if (cnt_opp_ball == 1) {
          carry.set_voltage(45);
          intake.set_voltage(-100);
          cout << "one opp ball\n";
        }
        // if (cnt_opp_ball == 2) {
        //   carry.set_voltage(-55);
        //   intake.set_voltage(-100);
        //   cout << "two opp ball\n";
        //   wait(500);
        // }

        // carry my ball up
        timer t;
        while (t.time() < 1000) {
          if (BALL_AT_CARRIER_END)
            carry.stop();
          wait(5);
        }
        t.reset();
        while_wait(t.time() < 500 && !key_collect_in && !key_collect_in &&
                   !key_shoot_only_manually &&
                   !key_shoot_continously_on_intaking);
        intake.stop();
      }
    }
    wait(50);
  }
}