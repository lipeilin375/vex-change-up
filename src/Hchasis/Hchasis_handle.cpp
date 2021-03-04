#include "Hchasis.h"

void HChasis::handle(double chx_forward, double chx_turn, bool slow_mode,
                        ctrlMode mode, double max_forward_pct,
                        double max_turn_pct) {

  ActionUnit::all_units_stop_on_working( abs(chx_forward)>20 || abs(chx_turn)>20);

  zero_out_blind(chx_forward, 3);
  zero_out_blind(chx_turn, 3);

  //摇杆数值处理，稳定小数值归零
  chx_forward = process.zero_out_stable_small_value(
      chx_forward, JoystickValueProcess::timer_index_forward);
  chx_turn = process.zero_out_stable_small_value(
      chx_turn, JoystickValueProcess::timer_index_turn);

  if (slow_mode) {
    double s_forward = chx_forward / 100.0 * 20;
    double s_turn = chx_turn / 100.0 * 20;

    if (mode == m_velocity)
      FT_set_speed(s_forward, s_turn);
    else
      FT_set_voltage(s_forward, s_turn);
  } else {
    chx_forward = chx_forward / 100.0 * max_forward_pct;
    chx_turn = chx_turn / 100.0 * max_turn_pct;

    if (mode == m_velocity)
      FT_set_speed(chx_forward, chx_turn);
    else
      FT_set_voltage(chx_forward, chx_turn);
  }
}
