#include "Hchasis.h"

void HChasis::turn_enc_absolute(double rotation, double speed, int timeout_ms,
                                bool waitForCompletion, rotationUnits units_r,
                                velocityUnits units_v) {
  // reset_enc();//编码器清零
  p_au_r->act_for_enc_absolute(-rotation, speed, timeout_ms, false, 0, units_r,
                               units_v);
  p_au_l->act_for_enc_absolute(rotation, speed, timeout_ms, waitForCompletion,
                               0, units_r, units_v);
  wait(1);
}

void HChasis::turn_enc_relative(double rotation, double speed, int timeout_ms,
                                bool waitForCompletion, rotationUnits units_r,
                                velocityUnits units_v) {
  p_au_l->act_for_enc_relative(rotation, speed, timeout_ms, false, 0, units_r,
                               units_v);
  p_au_r->act_for_enc_relative(-rotation, speed, timeout_ms, waitForCompletion,
                               0, units_r, units_v);
  wait(1);
}
