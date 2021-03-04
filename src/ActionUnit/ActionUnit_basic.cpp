#include "ActionUnit.h"
vector<ActionUnit *>
    ActionUnit::vec_ptr_all_instacnes; //定义所有产生的实例的指针

void ActionUnit::all_units_reset_enc() {
  for (auto iter : vec_ptr_all_instacnes)
    iter->reset_enc();
}

void ActionUnit::all_units_stop() {
  for (auto iter : vec_ptr_all_instacnes)
    iter->stop();
}

////////////////////////////////////////////////////////////////////
bool ActionUnit::limit_pressed_max() {
  if (limit_max)
    return limit_max->pressing();
  else {
    Critical_Error_Null_Ptr("ActionUnit::limit_pressed_max");
    return false;
  }
}

bool ActionUnit::limit_pressed_min() {
  if (limit_min)
    return limit_min->pressing();
  else {
    Critical_Error_Null_Ptr("ActionUnit::limit_pressed_min");
    return false;
  }
}

void ActionUnit::reset_enc() { mtr_group.resetRotation(); }

double ActionUnit::get_enc(int motor_index, rotationUnits unit) {
  if (motor_index > get_motor_count() - 1 || motor_index < 0)
    return -1;
  wait(1); //短暂延时，防止连续读取本函数,用于cout打印显示不出来
  return vec_ptr_motors[motor_index]->position(unit);
}

double ActionUnit::get_velocity(int motor_index, velocityUnits units_v) {
  if (motor_index > get_motor_count() - 1 || motor_index < 0)
    return -1;
  return vec_ptr_motors[motor_index]->velocity(units_v);
}

////////////////////////////////////////////////////////////////////////
void ActionUnit::set_voltage(double pct_voltage) {
  mtr_group.spin(directionType::fwd, 12.8 * pct_voltage / 100.0,
                 voltageUnits::volt);
  set_speed_pct = 0;
  set_voltage_pct = pct_voltage;
}

void ActionUnit::set_voltage(double voltage, voltageUnits unit) {
  mtr_group.spin(directionType::fwd, voltage, unit);

  if (unit == voltageUnits::mV)
    set_voltage_pct = voltage / 1000.0 / 12.8 * 100;
  else
    set_voltage_pct = voltage / 12.8 * 100;
}

void ActionUnit::set_speed(double speed, velocityUnits unit) {
  mtr_group.spin(directionType::fwd, speed, unit);

  set_voltage_pct = 0;
  if (unit == velocityUnits::pct)
    set_speed_pct = speed;
  else
    set_speed_pct = 0; //*****其他单位直接set_speed_pct为0不想写了，就这样吧~
}

double ActionUnit::get_power_pct() { //速度或者电压
  if (set_voltage_pct != 0)
    return set_voltage_pct;
  if (set_speed_pct != 0)
    return set_speed_pct;
  return 0;
}

void ActionUnit::set_stopping(brakeType bk_type) {
  set_brake_type = bk_type;
  mtr_group.setStopping(set_brake_type);
}

bool ActionUnit::b_relax_static = true;

void ActionUnit::stop() {
  set_voltage_pct = 0;
  set_speed_pct = 0;

  if (false_act_enc_running) {
    false_act_enc_running = false; //停止waitForCompletion均设置为false的线程
  }

  if (b_relax_static)
    mtr_group.stop(brakeType::coast);
  else
    mtr_group.stop(set_brake_type);
}

void ActionUnit::low_voltage_resisting(int low_voltage_pct) {
  if (_abs(low_voltage_pct) >= 20)
    low_voltage_pct = _sgn(low_voltage_pct) * 20;
  if (b_relax_static)
    mtr_group.stop(brakeType::coast);
  else
    set_voltage(low_voltage_pct);
}

void ActionUnit::stop(brakeType bk_type) {
  mtr_group.stop(bk_type);
  set_voltage_pct = 0;
  set_speed_pct = 0;
}

void ActionUnit::brake_stop() { stop(brakeType::brake); }
void ActionUnit::coast_stop() { stop(brakeType::coast); }
void ActionUnit::hold_stop() { stop(brakeType::hold); }
