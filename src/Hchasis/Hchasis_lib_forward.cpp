#include "Hchasis.h"

void HChasis::forward_enc_absolute(double rotation, double speed,
                                   int timeout_ms, bool waitForCompletion,
                                   rotationUnits units_r,
                                   velocityUnits units_v) {
  p_au_l->act_for_enc_absolute(rotation, speed, timeout_ms, false, 0, units_r,
                               units_v);
  p_au_r->act_for_enc_absolute(rotation, speed, timeout_ms, waitForCompletion,
                               0, units_r, units_v);
                               wait(1);
}

void HChasis::forward_enc_relative(double rotation, double speed,
                                   int timeout_ms, bool waitForCompletion,
                                   rotationUnits units_r,
                                   velocityUnits units_v) {
  p_au_l->act_for_enc_relative(rotation, speed, timeout_ms, false, 0, units_r,
                               units_v);
  p_au_r->act_for_enc_relative(rotation, speed, timeout_ms, waitForCompletion,
                               0, units_r, units_v);
                               wait(1);

}
//////////////////////////////////////////////////////////////////////////////////
void HChasis::forward_dist_absolute(double dist, double speed, int timeout_ms,
                                    bool waitForCompletion,
                                    distanceUnits units_d,
                                    velocityUnits units_v) {
  double shaft_aim_rev = distance_to_rev(dist, units_d);
  // cout<<"shaft_aim_rev:"<<shaft_aim_rev<<endl;
  shaft_aim_rev*=0.960;

  forward_enc_absolute(shaft_aim_rev, speed, timeout_ms, waitForCompletion,
                       rotationUnits::rev, units_v);
                               wait(1);


}

void HChasis::forward_dist_relative(double dist, double speed, int timeout_ms,
                                    bool waitForCompletion,
                                    distanceUnits units_d,
                                    velocityUnits units_v) {
  double shaft_aim_rev = distance_to_rev(dist, units_d);
  forward_enc_relative(shaft_aim_rev, speed, timeout_ms, waitForCompletion,
                       rotationUnits::rev, units_v);
                               wait(1);

}