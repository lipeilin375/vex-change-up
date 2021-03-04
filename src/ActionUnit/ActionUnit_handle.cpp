#include "ActionUnit.h"

void ActionUnit::handle_key(bool key_fwd, bool key_rev, double pwr_pct_fwd,
                            double pwr_pct_rev, ctrlMode mode) {
  ActionUnit::all_units_stop_on_working(key_fwd || key_rev);

  if (key_fwd && !key_rev) {
    if (limit_pressed_max())
      stop();
    else {
      if (mode == ctrlMode::m_velocity)
        set_speed(pwr_pct_fwd);
      else
        set_voltage(pwr_pct_fwd);
    }
  } else if (key_rev && !key_fwd) {
    if (limit_pressed_min())
      stop();
    else {
      if (mode == ctrlMode::m_voltage)
        set_voltage(pwr_pct_rev);
      else
        set_speed(pwr_pct_rev);
    }
  } else
    stop();
}

//////////////////////////////////////////////////////////////////////////////////
void ActionUnit::handle_axis(int axis_value, double max_pwr,
                             unsigned int axis_threshold, ctrlMode mode) {
  ActionUnit::all_units_stop_on_working(_abs(axis_value) > 20);

  if (abs(axis_value) < abs(axis_threshold))
    axis_value = 0;

  if (axis_value != 0) {
    double pwr = max_pwr * (axis_value / 100.0);
    int sign = sgn(axis_value);

    if (sign == 1 && limit_pressed_max())
      stop();
    else if (sign == -1 && limit_pressed_min())
      stop();
    else {
      if (mode == ctrlMode::m_voltage)
        set_voltage(pwr);
      else
        set_speed(pwr);
    }
  } else
    stop();
}