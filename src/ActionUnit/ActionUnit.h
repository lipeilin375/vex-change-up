#pragma once
#define ActionUnit_Version 20200817

/*----------------------------------------------------------------------------
Module  : ActionUnit.h
Author  : LQ
Created : 2020.2.21
Last modified:2020.8.17
brief   : 本类是对V5自带的motor_group类进行二次封装。
          根据多年参赛经验,去掉motor_group类一些冗余的功能,并添加更多实用的功能。
          完全理解ActionUnit类的定义需要较高的C++语法知识,
          读者可自行参考相关资料。
----------------------------------------------------------------------------*/
/*
change log:
2020.11.1 增加is_all_units_relaxing()函数。

*/
#include "string"
#include "vector"
#include "vex.h"

//改写cout,方便调试
#define DEBUG_COUT                                                             \
  if (0)                                                                       \
  cout
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//动作单元的类定义,动作单元包含一个马达组
class ActionUnit {

public:
  static vector<ActionUnit *> vec_ptr_all_instacnes; //所有产生的实例的指针
  vex::motor_group mtr_group;                        //内部马达组
  vector<vex::motor *> vec_ptr_motors; //马达指针,用以保存各个马达

private:
  vex::limit *limit_max = NULL; //限位开关指针
  vex::limit *limit_min = NULL;

  // act data
  struct Act_Time_Data {
    unsigned int msec;
    double speed;
    velocityUnits units_v;
    brakeType brake_type;
  } act_time_data; //跑时间的参数结构体

  bool act_time_restart = false;
  bool act_time_running = false;

  struct Act_Enc_Data {
    double rotation;
    rotationUnits units_r;
    double speed;
    velocityUnits units_v;
    int timeout_ms;
    bool absolute_mode;
    int delay_ending_time; //后台线程延迟结束时间,用于不等待完成的情况
  } act_enc_data; //跑编码器的参数结构体
  bool false_act_enc_running = false;
  bool true_act_enc_running = false;

  string unit_name = "ActionUnit";

  double set_voltage_pct = 0;
  double set_speed_pct = 0;

private:
  brakeType set_brake_type = brakeType::brake; //默认刹停模式
  static bool b_relax_static;

private:
  //存储马达指针到vec_ptr_motors的函数
  void _addMotorToVector() {} //递归结束函数

  template <typename... Args> void _addMotorToVector(motor &m0, Args &... m) {
    vec_ptr_motors.push_back(&m0);
    _addMotorToVector(m...);
  }

public:
  //构造函数,所有参数都为马达
  template <typename... Args>
  ActionUnit(motor &m0, Args &... m) : mtr_group(m0, m...) {
    _addMotorToVector(m0, m...);
    vec_ptr_all_instacnes.push_back(this); //保存本实例指针
    set_stopping(set_brake_type);          //设置默认刹停模式
  }

  //构造函数,第一个参数为动作单元名字,后续参数都为马达
  template <typename... Args>
  ActionUnit(string n, Args &... m) : mtr_group(m...), unit_name(n) {
    _addMotorToVector(m...);
    vec_ptr_all_instacnes.push_back(this); //保存本实例指针
    set_stopping(set_brake_type);          //设置默认刹停模式
  }
  //////////////////////////////////////////////////////////////////////////////
private:
  /*内部使用的数学函数*/
  //符号函数
  int _sgn(double x) {
    if (x > 0)
      return 1;
    else if (x < 0)
      return -1;
    else
      return 0;
  }

  double _abs(double x) { return (((x) > 0) ? (x) : -(x)); } //绝对值函数
  //调用所有实例的成员函数
public:
  static void all_units_reset_enc();
  static void all_units_stop();

public:
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  motor_group &get_motor_group() {
    return mtr_group;
  } //获取内部motor_group变量mtr_group,用于特殊用途

  void print_unit_name() { cout << unit_name << endl; }
  void set_name(string name) { unit_name = name; };
  string get_name() { return unit_name; }

  //最小最大限位开关
  void set_limit_max(limit &lmt) { limit_max = &lmt; }
  void set_limit_min(limit &lmt) { limit_min = &lmt; }
  bool limit_pressed_max();
  bool limit_pressed_min();

  int get_motor_count() { return mtr_group.count(); }

  /*
    1.每转脉冲数:
    每转脉冲数是马达输出轴转1圈, 编码器的数值。
    定义每转脉冲数为rtpr(raw ticks per revolution)
     红齿 36:1 - rtpr=1800
     绿齿 18:1 - rtpr=900
     蓝齿  6:1 - rtpr=300

    2.单位转换公式:
    (raw) / rtpr =  (rev)
    (rev) * 360 =  (deg)
  */
  double get_enc(int motor_index = 0, rotationUnits unit = rotationUnits::raw);
  void reset_enc();

  double get_velocity(int motor_index = 0,
                      velocityUnits units_v = velocityUnits::pct);
  motor get_first_motor() { return *vec_ptr_motors[0]; }

  //根据设定马达电压百分百转动马达组,满电压为12.8伏
  void set_voltage(double pct_voltage);
  //根据数值和单位设定的电压转动马达组
  void set_voltage(double voltage, voltageUnits unit);

  double get_voltage_pct() { return set_voltage_pct; }
  double get_speed_pct() { return set_speed_pct; }
  double get_power_pct();
  /*
    根据设定马达速度百分百转动马达组
    红齿36:1 - 100rpm
    绿齿18:1 - 200rpm
    蓝齿 6:1 - 600rpm
 */
  void set_speed(double speed, velocityUnits unit = velocityUnits::pct);

  void set_stopping(brakeType bk_type); //设定刹停模式

  //静态函数,用以控制默认stop()函数,所有实例的stop()均生效
  static void all_units_stop_on_relaxing(bool condition = true) {
    if (condition && !b_relax_static)
      b_relax_static = true;
  };
  static void all_units_stop_on_working(bool condition = true) {
    if (condition && b_relax_static)
      b_relax_static = false;
  };

  static bool is_all_units_relaxing() { return b_relax_static; }

  void stop(); //使用设定的刹停模式刹停

  void stop(brakeType bk_type); //根据指定的刹停模式临时刹停

  void low_voltage_resisting(int voltage_pct); //低电压抵住,最大20（百分百）

  //临时刹停
  void brake_stop();
  void coast_stop();
  void hold_stop();
  ///////////////////////////////////////////////////////////////////////////////
public:
  // act_for_enc新的指令会等待前一个尚未结束的后台act_for_enc_service_cb_static程序。
  bool act_for_enc_absolute(double rotation, double speed, int timeout_ms = 0,
                            bool waitForCompletion = true,
                            int delay_ending_time = 0,
                            rotationUnits units_r = rotationUnits::raw,
                            velocityUnits units_v = velocityUnits::pct);
  bool act_for_enc_relative(double rotation, double speed, int timeout_ms = 0,
                            bool waitForCompletion = true,
                            int delay_ending_time = 0,
                            rotationUnits units_r = rotationUnits::raw,
                            velocityUnits units_v = velocityUnits::pct);
  bool is_acting_enc();

private:
  bool _act_for_enc(bool absolute_mode, double rotation, double speed,
                    int timeout_ms, bool waitForCompletion,
                    int delay_ending_time, rotationUnits units_r,
                    velocityUnits units_v);
  static void act_for_enc_service_cb_static(void *p_this);
  ///////////////////////////////////////////////////////////////////////////////
public:
  // act_for_time新的指令会强制停止前一个尚未结束的后台act_for_time_service_cb_static程序。
  void act_for_time(unsigned int msec, double speed,
                    bool waitForCompletion = false,
                    velocityUnits units_v = velocityUnits::pct,
                    brakeType brake_type = brakeType::brake);

  bool is_acting_time();

private:
  // class里的线程回调函数必须为static函数,并且不能访问类的non-static函数和变量
  static int act_for_time_service_cb_static(void *p_this);
  ///////////////////////////////////////////////////////////////////////////////
public:
  enum ctrlMode { m_voltage, m_velocity };
  //简单的遥控按键操控函数
  void handle_key(bool key_fwd, bool key_rev, double pwr_pct_fwd = 100,
                  double pwr_pct_rev = -100, ctrlMode mode = m_velocity);

  //简单的遥控摇杆操控函数, axis_value在-100~100之间
  void handle_axis(int axis_value, double max_pwr = 100,
                   unsigned axis_threshold = 5, ctrlMode mode = m_velocity);
};
