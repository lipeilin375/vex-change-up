#pragma once
/*----------------------------------------------------------------------------
Module  : ActionUnit.h
Author  : LQ
Created : 2020.2.21
brief   : 本类是用于对摇杆的误差进行处理。可以剔除摇杆松后不能摆正的小数值。
不同于简单的盲区归零处理,zero_out_stable_small_value只会把盲区内，稳定的很小的数值归零。
----------------------------------------------------------------------------*/

#include "vex.h"

class JoystickValueProcess {
private:
  int JOYSTICK_STABLE_BLIND_AREA = 10;        //摇杆稳定态盲区
  static const int JOYSTICK_VALUE_COUNT = 3; //控制底盘需要的摇杆通道数量
  
private:
  //静态变量,三个计时器
  vex::timer t[JOYSTICK_VALUE_COUNT];
  //静态变量，三个数值，用以保存前一次摇杆的数值
  int value_last[JOYSTICK_VALUE_COUNT];

public:
  static const int timer_index_forward = 0;
  static const int timer_index_turn = 1;
  static const int timer_index_sideway = 2;

  //稳定小数值归零处理函数
  //////////////////////////////////////////////////////////////////////////////////////////////
  int zero_out_stable_small_value(int value_new, int index) {
    int ret;
    if (abs(value_new) > JOYSTICK_STABLE_BLIND_AREA) { //盲区以外
      t[index].clear();
      ret = value_new;
    } else { //在盲区以内
      int sudden = abs(value_last[index] - value_new);
      if (sudden >= 3) { //数值突变
        t[index].clear();
        ret = value_new;
      } else {
        if (t[index].time() > 500) //在盲区中数值稳定超过500毫秒
          ret = 0;
        else
          ret = value_new;
      }
    }
    value_last[index] = value_new;
    return ret;
  }
};
