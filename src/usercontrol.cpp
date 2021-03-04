#include "_includes.h"

void _usercontrol(void) {

  thread_autonomous.interrupt(); // stop thread_autonomous
  autonomous_running = false;
  
  thread t(balls_dealing_handle);
  while (1) {
    if (!autonomous_running) {
      // encoders reset
      if (Controller.ButtonRight.pressing()) {
        screen_clear();
        buttons_show_up();
        ActionUnit::all_units_reset_enc();
        imu.resetRotation();
      }

      ///////////////////////////////////////////////
      // balls_dealing_handle();
      chasis.handle(1.3*Controller.Axis3.position(), 1.3*Controller.Axis4.position(),
                    false, HChasis::ctrlMode::m_voltage, 100, 100);
      special_key_handle();
    }
    wait(50); // handle interval
  }           // while(1)
}
