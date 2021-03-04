#include "_includes.h"

#define FORCE_STOP_AUTO true

#define RUN_SKILLS false

#if RUN_SKILLS
#define AUTO_TIME 60000
#else
#define AUTO_TIME 15000
#endif

void auto_thread_routine() {
  if (1) {
    auto_test();
    return;
  }

  if (RUN_SKILLS) {
    auto_skills();
  } else { // tournament auto
    if (auto_info.role == ROLE_LEFT) {
      if (auto_info.route_number == 1)
        auto_role_left_1();
      if (auto_info.route_number == 2)
        auto_role_left_2();
      if (auto_info.route_number == 3)
        auto_role_left_3();
    } 

    if (auto_info.role == ROLE_RIGHT) {
      if (auto_info.route_number == 1)
        auto_role_right_1();
      if (auto_info.route_number == 2)
        auto_role_right_2();
      if (auto_info.route_number == 3)
        auto_role_right_3();
    }
  }
}

bool autonomous_running = false;
vex::thread thread_autonomous;
void _autonomous(void) {
  vex::timer t_auto;
  autonomous_running = true;
  thread_autonomous = thread(auto_thread_routine); // start auto thread

  wait(150);
  screen_clear();

  Controller.Screen.clearLine(2);
  Controller.Screen.clearLine(3);

#if (FORCE_STOP_AUTO)
  while (t_auto.time() < AUTO_TIME && thread_autonomous.joinable()) {
    wait(50);
    Controller.Screen.setCursor(3, 1);
    Controller.Screen.print("auto time: %.1f  ", t_auto.time() / 1000.0);
  }
  thread_autonomous.interrupt(); // stop auto thread
  ActionUnit::all_units_stop();
#else
  while (thread_autonomous.joinable()) {
    wait(100);
    Controller.Screen.setCursor(3, 1);
    Controller.Screen.print("auto time: %.1f  ", t_auto.time() / 1000.0);
  }
#endif

  Controller.Screen.setCursor(3, 1);
  Controller.Screen.print("Auto Time: %.2fs   ", t_auto.time() / 1000.0);
  // cout << "auto time:" << t_auto.time() << endl << endl << endl;

  autonomous_running = false;
}
