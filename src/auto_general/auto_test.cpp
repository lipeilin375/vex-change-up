#include "../auto/auto.h"

extern vision vis_front;
//closer->Y bigger
int xxxxx(vision &v, signature &sig) {
  v.takeSnapshot(sig, 2);
  int n = v.objectCount;
  if (n == 0) {
    cout << "No object\n";
    return 0;
  } else {
    cout << "cnt: " << n << endl;
    int max_y = -10000;
    int max_k = 0;
    int k;
    for (k = 0; k < n; k++) {
      int y = v.objects[k].centerY;
      if (y > max_y) {
        max_y = y;
        max_k = k;
      }
    }
    // max_y object fiound
    int cx = v.objects[max_k].centerX;
    int dx = cx - 160;
    // cout << "maxy dx:" << v.objects[max_k].centerX-160 << endl;
    // cout << "maxy y:" << v.objects[max_k].centerY << endl;
    return dx;
  }
  return 0;
}

void auto_test() {
  while (1) {
    double dx=xxxxx(vis_front, vis__BLUE_BALL);
    max_limit(dx, 50);
    chasis.FT_set_voltage(0, dx);
    wait(10);
  }
}