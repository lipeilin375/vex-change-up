#include "vex.h"

void wait(int milsec) { vex::this_thread::sleep_for(milsec); }

int sgn(double x) {
  if (x > 0)
    return 1;
  else if (x < 0)
    return -1;
  else
    return 0;
}

void max_limit(double &x, double max_bnd) {
  if (abs(x) > max_bnd)
    x = sgn(x) * max_bnd;
}

void min_limit(double &x, double min_bnd) {
  if (abs(x) < min_bnd)
    x = sgn(x) * min_bnd;
}

void zero_out_blind(double &x, double threshold) {
  if (abs(x) < threshold)
    x = 0;
}
