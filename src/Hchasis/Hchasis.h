#pragma once
#define Hchasis_Version 20201017

#include "../ActionUnit/ActionUnit.h"
#include "joystick_value_process.h"

/////////////////////////
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

  3.路程与编码器
  （rev） / _external_gear_reduction_ratio  *   _wheel_travel  =  dist

*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////
class HChasis {
private:
  JoystickValueProcess process;
  ActionUnit *p_au_l = NULL;
  ActionUnit *p_au_r = NULL;

public:
  double _external_gear_reduction_ratio; //外部齿轮减速比,默认
  double _wheel_travel;                  //轮子周长,默认4英寸直径轮
  double _track_width = 324;             //左右轮距
  double _wheel_base = 320;              //前后轮轴距

public:
  //使用马达组构建底盘
  HChasis(ActionUnit &l, ActionUnit &r, double wheel_travel,
          double external_gear_ratio, float track_width = 324) {
    p_au_l = &l;
    p_au_r = &r;
    _wheel_travel = wheel_travel;
    _external_gear_reduction_ratio = external_gear_ratio;
  }
  ////////////////////////////////////////////////////////////////////////////////
  double get_velocity(int side, int mtr_index = 0,
                      velocityUnits units_v = velocityUnits::pct);
  double get_enc(int side, rotationUnits unit_r = rotationUnits::raw);
  double get_dist_mm(int side, int mtr_index = 0);

  void reset_enc();

  void set_stopping(brakeType bk_type); //设定刹停模式
  void stop(brakeType bk_type);
  void stop();

  void LR_set_voltage(double mv_pct_l, double mv_pct_r);
  void LR_set_speed(double v_pct_l, double v_pct_r);
  void FT_set_voltage(double forward_pct, double turn_pct);
  void FT_set_speed(double forward_pct, double turn_pct);
  ////////////////////////////////////////////////////////////////

  void forward_enc_absolute(double rotation, double speed, int timeout_ms = 0,
                            bool waitForCompletion = true,
                            rotationUnits units_r = rotationUnits::raw,
                            velocityUnits units_v = velocityUnits::pct);
  void forward_enc_relative(double rotation, double speed, int timeout_ms = 0,
                            bool waitForCompletion = true,
                            rotationUnits units_r = rotationUnits::raw,
                            velocityUnits units_v = velocityUnits::pct);
  void forward_dist_absolute(double dist, double speed, int timeout_ms = 0,
                             bool waitForCompletion = true,
                             distanceUnits units_d = distanceUnits::mm,
                             velocityUnits units_v = velocityUnits::pct);
  void forward_dist_relative(double dist, double speed, int timeout_ms = 0,
                             bool waitForCompletion = true,
                             distanceUnits units_d = distanceUnits::mm,
                             velocityUnits units_v = velocityUnits::pct);
  ////////////////////////////////////////////////////////////////

  void turn_enc_absolute(double rotation, double speed, int timeout_ms = 0,
                         bool waitForCompletion = true,
                         rotationUnits units_r = rotationUnits::raw,
                         velocityUnits units_v = velocityUnits::pct);
  void turn_enc_relative(double rotation, double speed, int timeout_ms = 0,
                         bool waitForCompletion = true,
                         rotationUnits units_r = rotationUnits::raw,
                         velocityUnits units_v = velocityUnits::pct);
  bool is_moving() {
    return p_au_l->is_acting_enc() && p_au_r->is_acting_enc();
  }
  ////////////////////////////////////////////////////////////////////////////////
  enum ctrlMode { m_voltage, m_velocity };
  void handle(double chx_forward, double chx_turn, bool slow_mode,
              ctrlMode mode = m_voltage, double max_forward_pct = 100,
              double max_turn_pct = 100);

public:
  double distance_to_rev(double distance, distanceUnits units);
  double rev_to_raw(double r, gearSetting s = gearSetting::ratio18_1);
  double rev_to_distance_mm(double r);

  ///////////////////////
private:
  float get_inner_spd(float vo, float radius_center, float tw);

public:
  void curve_move();
  void arc_move(int side, int spd_pct_outter_arc, double outter_arc_dist_mm,
                float radius_center);

  // forward_pid_adjust_dist_relative
public:
  bool forward_pid_adjust_force_stop = false; //强制停止
  bool forward_pid_adjust_running = false;    //标志，正在运行

  struct {
    bool run_done; //状态标志，是否完成
    double aim_dist_mm;
    double max_pwr;
    int timeout;
    bool waitForCompletion;
    bool b_remove_gearspace;
    bool b_reset_enc;
    double max_adjust_pwr;
  } arg_forward_pid_adjust_dist_relative;

  static void forward_pid_adjust_dist_relative_data_cb_static(void *p_this);

public:
  double remove_gearspace(int dir);
  void point_move_relative(int dist_ralative = 5, int timeout = 100);
  void forward_pid_adjust_dist_relative(double aim_dist_mm,
                                        double max_pwr = 100, int timeout = 0,
                                        bool waitForCompletion = true,
                                        bool b_remove_gearspace = true,
                                        bool b_reset_enc = true,
                                        double max_adjust_pwr = 15);
  bool is_forward_pid_adjust_done() {
    return arg_forward_pid_adjust_dist_relative.run_done;
  }
};
