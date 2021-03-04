#include "BtnEvent.h"
#include "BtnShape.h"
////////////////////////////////////////////////////
bool BtnEvent::thread_already_created = false;
bool BtnEvent::thread_resting = true;

/************************************************
构造函数
*/
BtnEvent::BtnEvent(BtnShape *pbtn) : _p_parent_btn(pbtn) {
  //后台线程相关
  while (!thread_resting) { //等待线程停止运行时再增加
    wait(1);
  }

  //创建线程
  if (!thread_already_created) {
    thread_already_created = true;
    thread t(all_buttons_update_and_run_callback_thread_routine);
  }
}

/************************************************
析构函数
*/
BtnEvent::~BtnEvent() { _p_parent_btn = NULL; }

/************************************************
更新按钮状态函数
*/
void BtnEvent::update_status_data() {
  curSta = _p_parent_btn->pressing();

  if (handle_press_trig) {
    trig_pressed = curSta & (curSta ^ lastSta); //按下, 瞬间触发
    //按下挂起标志置位,不会重复挂起
    if (!trig_pressed_pending_flag && trig_pressed)
    {
      trig_pressed_pending_flag = true;
    }
  }

  if (handle_release_trig) {
    trig_released = !curSta & (curSta ^ lastSta); //松开, 瞬间触发
    //按下挂起标志置位,不会重复挂起
    if (!trig_released_pending_flag && trig_released)
      trig_released_pending_flag = true;
  }
  lastSta = curSta; //保存上一次的状态
}

/************************************************
执行回调函数
*/
void BtnEvent::execute_callback() {
  if (handle_press_trig && trig_pressed_pending_flag) {
    if (pressed_cb != NULL)
      pressed_cb(_p_parent_btn);
    clear_press_pending_flag();
  }

  if (handle_release_trig && trig_released_pending_flag) {
    if (released_cb != NULL)
      released_cb(_p_parent_btn);
    clear_release_pending_flag();
  }
}

/************************************************
按钮状态更新和执行回调函数的线程函数
*/
void BtnEvent::all_buttons_update_and_run_callback_thread_routine() {
  int k = 0;
  while (1) {

    // while (BtnShape::get_vec_ptr_all_btns()_changing)
    //   wait(1);

    thread_resting = true;
    wait(20);

    thread_resting = false;
    for (auto iter : BtnShape::get_vec_ptr_all_btns())
      iter->ent.update_status_data(); // 20毫秒更新一次状态

    // cout<<"size:"<<BtnShape::get_vec_ptr_all_btns().size()<<endl;

    if (k == 2) {
      for (auto iter : BtnShape::get_vec_ptr_all_btns())
        iter->ent.execute_callback(); // 40毫秒一次执行回调函数
      k = 0;
    }
    k++;
  }
}
