#pragma once
#include <iostream>
using namespace std;
/**********************************************
BtnEvent类用来保存回调函数、更新按钮的状态以及执行回调函数
*/

class BtnShape; //前向声明

typedef void (*pBtnCallback)(BtnShape *p_this_object); //回调函数形式

//内部类定义
class BtnEvent {
public:
  BtnShape *_p_parent_btn = NULL;
  pBtnCallback pressed_cb = NULL;
  pBtnCallback released_cb = NULL;

private:
  bool handle_press_trig = false, handle_release_trig = false;
  bool trig_pressed, trig_released;
  bool trig_pressed_pending_flag = false, trig_released_pending_flag = false;
  bool lastSta = false;
  bool curSta;

public:
  BtnEvent(BtnShape *pbtn);
  ~BtnEvent();

private:
  void clear_press_pending_flag() { trig_pressed_pending_flag = false; }
  void clear_release_pending_flag() { trig_released_pending_flag = false; }

public:
  ////注册按下回调函数
  void save_pressed_cb(pBtnCallback cb) {
    handle_press_trig = true;
    pressed_cb = cb;
  }

  ////注册松开回调函数
  void save_released_cb(pBtnCallback cb) {
    handle_release_trig = true;
    released_cb = cb;
  }

  ////取消按下回调函数
  void cancel_pressed_cb() {
    handle_press_trig = false;
    pressed_cb = NULL;
  }

  ////取消按下回调函数
  void cancel_released_cb() {
    handle_release_trig = false;
    released_cb = NULL;
  }

  void update_status_data();
  void execute_callback();

public:
  //按钮线程相关
  static bool thread_already_created;
  static bool thread_resting;

  static void all_buttons_update_and_run_callback_thread_routine();
};
