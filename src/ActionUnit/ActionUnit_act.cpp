#include "ActionUnit.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionUnit::act_for_enc_service_cb_static(void *p_this) {
  ActionUnit *p_this_object = (ActionUnit *)p_this;
  DEBUG_COUT << "false actor:" << p_this_object->get_name() << endl;
  //先复制出参数供后续使用,以免act_enc_data被覆盖
  Act_Enc_Data data = p_this_object->act_enc_data;

  //等待上一个waitForCompletion==false的后台线程结束
  while (p_this_object->false_act_enc_running) {
    wait(5);
  }

  p_this_object->false_act_enc_running = true;
  bool done = false;

  if (data.absolute_mode)
    p_this_object->mtr_group.spinToPosition(data.rotation, data.units_r,
                                            data.speed, data.units_v,
                                            false); //绝对位置模式
  else
    p_this_object->mtr_group.spinFor(data.rotation, data.units_r, data.speed,
                                     data.units_v,
                                     false); //相对位置模式
  timer t;
  while (1) { //等待超时
    if (p_this_object->false_act_enc_running ==
        false) { //执行了stop（）函数，false_act_enc_running被修改成false，来提前终止
      DEBUG_COUT << "forced stop!!!!" << endl;
      return; //被外部线程执行stop（）强制停转
    }

    if (data.timeout_ms != 0 && t.time() > data.timeout_ms) {
      DEBUG_COUT << "false time out stop!!!!" << endl;
      p_this_object->mtr_group.stop();
      break; //超时提前停转
    }

    done = p_this_object->mtr_group.isDone();
    if (done) {
      DEBUG_COUT << "false act_for_enc ok~~" << endl;
      break;
    }

    wait(5);
  }

  if (data.delay_ending_time) {
    DEBUG_COUT << "false act_for_enc delay ending:" << data.delay_ending_time
               << endl;
    wait(data.delay_ending_time); //延迟结束
  }

  p_this_object->false_act_enc_running = false; //正常结束标志
}
/*
spinToPosition和spinFor函数的超时设定(setTimeout函数)必须要等待完成(waitForCompletion为true)才生效。
*/
bool ActionUnit::_act_for_enc(bool absolute_mode, double rotation, double speed,
                              int timeout_ms, bool waitForCompletion,
                              int delay_ending_time, rotationUnits units_r,
                              velocityUnits units_v) {

  bool done = false;
  if (waitForCompletion) { //需要等待完成,阻塞型
    DEBUG_COUT << "true actor:" << get_name() << endl;

    //等待上一个waitForCompletion==false的后台线程结束
    while (false_act_enc_running) {
      wait(5);
    }

    true_act_enc_running = true;
    mtr_group.setTimeout(timeout_ms, timeUnits::msec);

    if (absolute_mode)
      done = mtr_group.spinToPosition(rotation, units_r, speed, units_v, true);
    else
      done = mtr_group.spinFor(rotation, units_r, speed, units_v, true);

    if (done) {
      DEBUG_COUT << "true act_for_enc ok~~" << endl;
    } else
      DEBUG_COUT << "true act_for_enc timeout..." << endl;
    true_act_enc_running = false;

  } else { //不等待完成(waitForCompletion为false),则启用后台线程来运行,非阻塞型
    this->act_enc_data = {rotation,         units_r,    speed,
                          units_v,          timeout_ms, absolute_mode,
                          delay_ending_time};      //准备线程运行参数
    thread t(act_for_enc_service_cb_static, this); //建立后台线程

    // 等待线程启动
    while (!false_act_enc_running) {
      wait(5);
    }

    done = false;
  }
  return done;
}

bool ActionUnit::act_for_enc_absolute(double rotation, double speed,
                                      int timeout_ms, bool waitForCompletion,
                                      int delay_ending_time,
                                      rotationUnits units_r,
                                      velocityUnits units_v) {
  return _act_for_enc(true, rotation, speed, timeout_ms, waitForCompletion,
                      delay_ending_time, units_r, units_v);
}

bool ActionUnit::act_for_enc_relative(double rotation, double speed,
                                      int timeout_ms, bool waitForCompletion,
                                      int delay_ending_time,
                                      rotationUnits units_r,
                                      velocityUnits units_v) {
  return _act_for_enc(false, rotation, speed, timeout_ms, waitForCompletion,
                      delay_ending_time, units_r, units_v);
}

bool ActionUnit::is_acting_enc() {
  return false_act_enc_running || true_act_enc_running;
  ;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*













*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*类静态函数作为后台运行函数,由各个实例开辟的独立线程运行
pp_this_object_ActionUnit为传递进来的实例的指针
*/
int ActionUnit::act_for_time_service_cb_static(void *p_this) {
  ActionUnit *p_this_object = (ActionUnit *)p_this;

  p_this_object->act_time_running = true;
  DEBUG_COUT << "actor: " << p_this_object->unit_name << endl;

  p_this_object->set_speed(p_this_object->act_time_data.speed);

  timer t;
  bool force_stop = false;
  while (t.time() < p_this_object->act_time_data.msec) {

    // force stop
    if (p_this_object->act_time_restart) {
      DEBUG_COUT << p_this_object->unit_name
                 << " act is forced to stop in advance" << endl;
      force_stop = true;
      break;
    }
    wait(5); //每5ms去检查一次是否时间到了
  }
  p_this_object->stop(p_this_object->act_time_data.brake_type);
  p_this_object->act_time_running = false;

  return (force_stop) ? 0 : 1;
}

void ActionUnit::act_for_time(unsigned int msec, double speed,
                              bool waitForCompletion, velocityUnits units_v,
                              brakeType bk_type) {
  // act_time_restart 改为 true,
  // 用以停止前一个运行act_for_time_service_cb_static
  act_time_restart = true;
  this->act_time_data = {msec, speed, units_v, bk_type}; //准备线程运行参数

  // 等待前一个act_time 线程结束
  while (act_time_running)
    wait(5);
  act_time_restart = false;
  //开辟线程,将本实例的this指针传递给act_for_time_service_cb_static,使其控制本实例
  thread t(act_for_time_service_cb_static, this);

  //等待新的act_time 线程启动
  while (!act_time_running) {
    DEBUG_COUT << endl << "act_time all set,wait for beginning" << endl;
    wait(5);
  }
  //等待线程结束
  if (waitForCompletion) {
    while (t.joinable())
      wait(10);
  }
}

bool ActionUnit::is_acting_time() {
  wait(1);
  return (!act_time_restart && act_time_running);
}