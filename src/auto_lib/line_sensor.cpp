#include "../_includes.h"
#include "auto_lib.h"

bool line_sensor_detected(line sensor, int threshold) {
  if (sensor.value(percentUnits::pct) == 0) // sensor not plugged in
    return false;
  if (sensor.value(percentUnits::pct) <= threshold)
    return true;
  return false;
}

////
void auto_carry_load_ball() {
  carry.set_voltage(60);
  timer t;
  while (!BALL_AT_CARRIER_END && t.time() < 400) {
    if (key_shoot_only_manually || key_shoot_continously_on_intaking ||
        key_collect_in || key_collect_out)
      break;
    wait(5);
  }
  carry.set_voltage(-2);
}
///////////////////////////////////////////////////////////////////////
void shoot_one_ball(bool get_next_ball_ready, bool stop_shooter,
                    bool carry_stop) {
  shoot.set_voltage(100);
  timer t;

  if (BALL_AT_SHOOTER) { // ball at shooter
    // cout << "BALL_AT_SHOOTER\n";

    t.reset();
    carry.set_voltage(80);
    while (BALL_AT_SHOOTER && t.time() < 500) {
      if (t.time() > 150)
        carry.set_voltage(30);
      if (BALL_AT_CARRIER_END) {
        // intake.stop();
        carry.stop();
        wait(200); // wait for 2st ball away
        break;
      }
      wait(2);
    }
  } else if (BALL_AT_CARRIER_END) { // ball at carrier
    // cout << "BALL_AT_CARRIER_END\n";

    carry.set_voltage(80);
    while_wait_timeout(BALL_AT_CARRIER_END, 300);
    carry.set_voltage(30);
    while_wait_timeout(BALL_AT_SHOOTER, 300); // shoot ball
  } else                                      // load ball
  {
    // cout << "BALL_loading...\n";
    carry.set_voltage(100);
    t.reset();
    bool load_ok = false;
    while (1) {
      if (BALL_AT_CARRIER_END || BALL_AT_SHOOTER) {
        load_ok = true;
        break;
      }
      if (t.time() > 300) {
        carry.stop();
        break;
      }
    }
    if (load_ok) {
      carry.set_voltage(30);
      while_wait_timeout(BALL_AT_CARRIER_END && !BALL_AT_SHOOTER, 300);
      while_wait_timeout(BALL_AT_SHOOTER, 300); // shoot ball
    }
  }

  t.reset(); // wait for ball completely shoot
  while (t.time() < 400) {
    if (BALL_AT_CARRIER_END) {
      // cout << "next ball loaded in advance\n";
      carry.stop();
    }
    wait(2);
  }

  if (get_next_ball_ready) {
    carry.set_voltage(40); // carry slow down
    while_wait_timeout(!BALL_AT_CARRIER_END, 500);
    // cout << "next ball loaded ok\n";
  }

  if (stop_shooter) {
    shoot.hold_stop();
  }
  if (carry_stop)
    carry.stop();
}

void shoot_one_ball_carry_not_stop() { shoot_one_ball(true, true, false); }
void shoot_one_ball_carry_stop() { shoot_one_ball(true, true, true); }

///////////////////////////////////////////////////////////////////////////
void shoot_one_ball_simple() {
  shoot.set_voltage(100);
  carry.set_voltage(100);
  while_wait_timeout(!BALL_AT_SHOOTER && !BALL_AT_CARRIER_END, 1500);
  while_wait_timeout(!BALL_AT_SHOOTER, 500);
  carry.set_voltage(20); // BALL_AT_SHOOTER
  while_wait_timeout(BALL_AT_SHOOTER, 500);
  carry.stop();
  wait(300);
  shoot.set_voltage(vtg_shoot_idling);
}