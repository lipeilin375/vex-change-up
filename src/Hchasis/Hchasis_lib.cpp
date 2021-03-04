#include "Hchasis.h"

double HChasis::get_enc(int side, rotationUnits unit_r) {
  if (side == -1)
    return p_au_l->get_enc(0, unit_r);
  if (side == 1)
    return p_au_r->get_enc(0, unit_r);
  return 0;
}

double HChasis::get_velocity(int side, int mtr_index, velocityUnits units_v) {
  if (side == -1)
    return p_au_l->get_velocity(mtr_index, units_v);
  if (side == 1)
    return p_au_r->get_velocity(mtr_index, units_v);
  return 0;
}

void HChasis::reset_enc() {
  p_au_l->reset_enc();
  p_au_r->reset_enc();
}

void HChasis::set_stopping(brakeType bk_type) {
  p_au_l->set_stopping(bk_type);
  p_au_r->set_stopping(bk_type);
}

void HChasis::stop(brakeType bk_type) {
  p_au_l->stop(bk_type);
  p_au_r->stop(bk_type);
  // forward_pid_adjust_force_stop = true;
}

void HChasis::stop() {
  p_au_l->stop();
  p_au_r->stop();
  forward_pid_adjust_force_stop= true;
}

/////////////////////////////////////////////////////////////////////
void HChasis::LR_set_voltage(double mv_pct_l, double mv_pct_r) {
  //调用ActionUnit的stop()函数，可以让机器底盘放松
  if (mv_pct_l != 0)
    p_au_l->set_voltage(mv_pct_l);
  else
    p_au_l->stop();

  if (mv_pct_r != 0)
    p_au_r->set_voltage(mv_pct_r);
  else
    p_au_r->stop();
}

void HChasis::LR_set_speed(double v_pct_l, double v_pct_r) {
  //调用ActionUnit的stop()函数，可以让机器底盘放松
  if (v_pct_l != 0)
    p_au_l->set_speed(v_pct_l);
  else
    p_au_l->stop();

  if (v_pct_r != 0)
    p_au_r->set_speed(v_pct_r);
  else
    p_au_r->stop();
}

void HChasis::FT_set_voltage(double forward_pct, double turn_pct) {
  LR_set_voltage(forward_pct + turn_pct, forward_pct - turn_pct);
}
void HChasis::FT_set_speed(double forward_pct, double turn_pct) {
  LR_set_speed(forward_pct + turn_pct, forward_pct - turn_pct);
}

/////////////////////////////////////////////////////////////////////////
double HChasis::distance_to_rev(double distance, distanceUnits units) {
  ///////////////////////
  distance = distance * 0.985; //参数修正，少跑

  double k;
  if (units == distanceUnits::cm)
    k = 10;
  else if (units == distanceUnits::in)
    k = 25.4;
  else
    k = 1;
  double dist = distance * k;
  double axis_rev = dist / _wheel_travel * _external_gear_reduction_ratio;
  return axis_rev;
}

double HChasis::rev_to_distance_mm(double r) {

  double dist = r / _external_gear_reduction_ratio * _wheel_travel;
  dist *= 0.985; //显示参数修正
  return dist;
}

double HChasis::rev_to_raw(double r, gearSetting s) {
  float k;
  if (s == gearSetting::ratio36_1)
    k = 1800;
  if (s == gearSetting::ratio18_1)
    k = 900;
  if (s == gearSetting::ratio6_1)
    k = 300;
  double raw_enc = k * r;
  return raw_enc;
}

double HChasis::get_dist_mm(int side, int mtr_index) {
  double dist;
  if (side == -1)
    dist = rev_to_distance_mm(p_au_l->get_enc(mtr_index, rotationUnits::rev));
  if (side == 1)
    dist = rev_to_distance_mm(p_au_r->get_enc(mtr_index, rotationUnits::rev));
  return dist;
}
