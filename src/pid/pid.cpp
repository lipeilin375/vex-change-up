#include "pid.h"

#include <iostream>

pid_ctrl::pid_ctrl(float kp, float ki, float kd, float max_II_put, float aim,
                   float blind_area, float II_threshold, float max_pwr) {

  this->max_II = max_II_put / ki;
  this->kp = kp;
  this->ki = ki;
  this->kd = kd; 
  this->max_pwr = max_pwr;
  this->max_II_put = max_II_put;
  this->aim = aim;
  this->blind_area = blind_area;
  this->II_threshold = II_threshold;
}

void pid_ctrl::reset() { II = 0; }

float pid_ctrl::pid_calc() {
  if (abs(err_now) <= blind_area)
    err_now = 0;
  PP_put = kp * err_now;

  if (abs(err_now) < II_threshold)
    II += err_now;

  if (abs(II) > max_II)
    II = sgn(II) * max_II;
  II_put = ki * II;
  if (abs(II_put) > max_II_put)
    II_put = sgn(II_put) * max_II_put;

  DD_put = kd * (err_last - err_now);

  puts = kp * err_now + II_put + DD_put;
  if (abs(puts) > max_pwr)
    puts = sgn(puts) * max_pwr;

  err_last = err_now;

  if (print_detail) {
    std::cout << "err:" << err_now << " output:" << puts << std::endl;
    std::cout << "  II_put:" << II_put << "  PP_put:" << PP_put
              << " output:" << puts << std::endl
              << std::endl;
  }
  return puts;
}

float pid_ctrl::pid_output(float value_now) {
  val_now = value_now;
  err_now = aim - val_now;
  return pid_calc();
}

float pid_ctrl::pid_output_need_value(float aim_now, float value_now) {
  aim = aim_now;
  val_now = value_now;
  err_now = aim - val_now;
  return pid_calc();
}

float pid_ctrl::pid_output_need_err(float err_src) {
  err_now = err_src;
  return pid_calc();
}
