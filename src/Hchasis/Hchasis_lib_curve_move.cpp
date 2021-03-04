#include "Hchasis.h"


// tw: track width
float HChasis::get_inner_spd(float vo, float radius_center, float tw) {
  float vi = vo * (2 * radius_center - tw) / (2 * radius_center + tw);
  return vi;
}

void HChasis::arc_move(int side, int spd_pct_outter_arc,
                       double outter_arc_dist_mm, float radius_center) {

  // set side info, side==1 by default
  motor mtr_outter_arc = p_au_l->get_first_motor();

  ActionUnit chasis_outter = *p_au_l;
  ActionUnit chasis_inner = *p_au_r;

  if (side == -1) {
    mtr_outter_arc = p_au_r->get_first_motor();
    chasis_outter = *p_au_r;
    chasis_inner = *p_au_l;
  }

  const float tw = _track_width; //左右轮距
  float rc = radius_center;
  cout << "arc dist sum:"
       << (outter_arc_dist_mm * (rc - tw / 2) / (rc + tw / 2)) +
              outter_arc_dist_mm
       << endl;

  float vi, vo; //内弧速度、外弧速度、中心速度
  vo = spd_pct_outter_arc;

  // slowly speeding to vo,vi
  float _vo = 0, actual_vo;

  double dist_begin = get_dist_mm(-side);
  while (1) {
    if (_vo >= vo)
      break;
    actual_vo = mtr_outter_arc.velocity(percentUnits::pct);
    vi = get_inner_spd(actual_vo, rc, tw);
    chasis_inner.set_speed(vi);
    chasis_outter.set_speed(_vo++);
    if (get_dist_mm(-side) - dist_begin > outter_arc_dist_mm)
      break;
    wait(1); 
  }

  dist_begin = get_dist_mm(-side);
  while (1) {
    actual_vo = mtr_outter_arc.velocity(percentUnits::pct);
    vi = get_inner_spd(actual_vo, rc, tw);
    chasis_inner.set_speed(vi);
    wait(1);

    if (get_dist_mm(-side) - dist_begin > outter_arc_dist_mm)
      break;
  }
  stop();
}
