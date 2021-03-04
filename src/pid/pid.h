#pragma once

#include "vex.h"

class pid_ctrl {
public:
  bool print_detail = false;

public:
  float aim = 0;

  float blind_area = 0;

  float kp;

private:
  float PP_put;

  float ki;
  float II_threshold;
  float max_II_put;
  float max_II;

  float II_put;
  float II = 0;

  float kd;
  float DD_put;

  float max_pwr;

  float puts = 0;
  float err_now = 0;
  float err_last = 0;

  float val_now = 0;

public:
  pid_ctrl(float kp, float ki, float kd, float max_II_put, float aim,
           float blind_area, float II_threshold, float max_pwr);

  void set_kp(float kp) { this->kp = kp; }
  void set_ki(float ki) { this->ki = ki; }
  void set_kd(float kd) { this->kd = kd; }
  void set_max_II_put(float max_II_put) { this->max_II_put = max_II_put; }
  void set_aim(float aim) { this->aim = aim; }
  void set_blind_area(float blind_area) { this->blind_area = blind_area; }
  void set_II_threshold(float II_threshold) {
    this->II_threshold = II_threshold;
  }
  void set_max_pwr(float max_pwr) { this->max_pwr = max_pwr; }

  void reset();

  float pid_calc();
  float get_err() { return err_now; }
  float get_II() { return II; }

  float pid_output(float value_now);

  float pid_output_need_value(float aim_now, float value_now);

  float pid_output_need_err(float err_src);

  void set_print_detail(bool p) { print_detail = p; }
};
