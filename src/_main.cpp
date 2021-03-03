#include "_includes.h"

//系统的变量
vex::brain Brain;
vex::controller Controller;
vex::competition Competition;
////////////////////////////////////////////////////////////////////////
unsigned int sec_n = 0;

int main() {
  Controller.Screen.clearScreen();
  pre_auton();

  if (1) {
    Competition.autonomous(_autonomous);
    Competition.drivercontrol(_usercontrol);
  }

  Clock clock;
  int k = 0;
  while (1) {
    if (clock.get_msec() > sec_n * 1000) {
      sec_n++;
      Controller.Screen.setCursor(1, 1);
      Controller.Screen.print("B: %d%%    T: %d:%02d  ",
                              (int)Brain.Battery.capacity(), clock.get_min(),
                              clock.get_sec());
    }

    if (++k >= 3 && !autonomous_running) {
      Controller.Screen.setCursor(2, 1);
      Controller.Screen.print("g:%.2f  d:%d mm          ", get_gyro(),
                              (int)chasis.get_dist_mm(-1));
      k = 0;
    }

    wait(100);
  }
}
