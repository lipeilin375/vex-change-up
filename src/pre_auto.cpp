#include "_includes.h"

void controller_print_team_number() {
  Controller.Screen.setCursor(3, 1);
  Controller.Screen.print(team_name.c_str());
}

////////////////////////////////////////////////////////////////////////////////

void pre_auton(void) {
  vis.setWifiMode(vision::wifiMode::on);
  wait(100);
  shoot.set_stopping(brakeType::brake);
  vis.setLedBrightness(100);
  vis.setLedColor(0, 0, 0);
  vis.setLedMode(vision::ledMode::manual);
  controller_print_team_number();
  vex::thread th_display(display_manager_thread_routine); //后台显示线程
}