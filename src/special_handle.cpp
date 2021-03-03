#include "_includes.h"
bool need_relaxing = true;

//后台测试自动，可以左键停止
void run_auto_background() {
  autonomous_running = true;
  ActionUnit::all_units_stop_on_working();
  vex::thread th_auto_test(_autonomous);
  wait(10); // wait for start
  // 等待自动程序完毕
  while (th_auto_test.joinable()) {
    if (Controller.ButtonLeft.pressing()) { //强制停止自动程序
      thread_autonomous.interrupt(); //停止场控启动的自动程序后台线程
      th_auto_test.interrupt();      // 停止自动程序后台线程
      ActionUnit::all_units_stop(); //所有运动单元停转
      chasis.stop();

      Controller.Screen.setCursor(3, 1);
      Controller.Screen.print("auto terminated");
      while (Controller.ButtonLeft.pressing())
        wait(5);
    }
    wait(250);
  } //自动程序测试完毕
  autonomous_running = false;
  ActionUnit::all_units_stop();
}

//底盘马达测试,按下转，松开停，再按反转
void chasis_motor_test(controller::button &b, motor &m, int &dir) {
  if (b.pressing()) {
    need_relaxing = false;
    m.spin(directionType::fwd, 12 * dir, voltageUnits::volt);
    dir = -dir;
    while_wait(b.pressing());
    m.stop();
  }
}

void special_key_handle() {
  autonomous_running = false;
  need_relaxing = true;
  if (Controller.ButtonLeft.pressing()) {
    ActionUnit::all_units_stop(); //所有运动单元停转

    int dir_lf = 1, dir_lb = 1, dir_rf = 1, dir_rb = 1; //底盘马达测试旋转方向

    while (Controller.ButtonLeft.pressing()) {
      /*
      1:自动程序测试
      */
      if (Controller.ButtonRight.pressing()) { // key right
        need_relaxing = false;
        while_wait(Controller.ButtonRight.pressing() ||
                   Controller.ButtonLeft.pressing()); // 等待松开
        run_auto_background();
      } // 1:自动程序测试

      /*
      2:底盘马达测试
      */
      if (1) {
        chasis_motor_test(Controller.ButtonUp, mtr_LF, dir_lf);
        chasis_motor_test(Controller.ButtonDown, mtr_LB, dir_lb);
        chasis_motor_test(Controller.ButtonX, mtr_RF, dir_rf);
        chasis_motor_test(Controller.ButtonB, mtr_RB, dir_rb);
      } // 2:底盘马达测试

      /*
      3:显示区页面切换
      */
      if (1) {
        int switch_value = 40;

        if (Controller.Axis2.position() >= switch_value) {
          need_relaxing = false;
          page_up();
          while_wait(Controller.Axis2.position() >= switch_value);
        }

        if (Controller.Axis2.position() <= -switch_value) {
          need_relaxing = false;
          page_down();
          while_wait(Controller.Axis2.position() <= -switch_value);
        }

      } // 3:显示区页面切换

    } // while(key left)
    /////////////////////////////
    // after
    if (need_relaxing) {
      Controller.Screen.clearLine(3);
      controller_print_team_number();
      ActionUnit::all_units_stop_on_relaxing();
    }
  } // if key left
}
