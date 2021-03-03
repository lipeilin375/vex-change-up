#include "../pid/pid.h"
#include "Hchasis.h"

/////////////////////////
/*
Module  : forward_pid_adjust_dist_relative.cpp
Author  : LQ
Created : 2020.10.19
Last modified:2020.10.19
brief   :
本函数使用陀螺仪和编码器进行前进后退运动,左侧编码器PID计算距离，陀螺仪PID进行方向矫正。
          共计5个阶段。 0： 去除齿轮空程，大约需要0.1秒。 1：自增匀加速阶段。
2："匀速"阶段,此阶段采用pid进行速度计算。 3： 自减匀减速阶段。
4：微距离点动最终达到设定值。
   其中由于齿轮有空程间隙，例如前进时，刹车阶段马达不转机器也会由于惯性继续向前运动，直到空程转完，因此设定的目标值需要减去这一段空程距离。
  空程距离大约30mm。
*/

#define COUT                                                                   \
  if (0)                                                                       \
  cout

#define simple_cout                                                            \
  if (0)                                                                       \
  cout

///////////////////////////////////////////////////////////////////////////////
/*
去除底盘齿轮空程
*/
///////////////////////////////////////////////////////////////////////////////
double HChasis::remove_gearspace(int dir) {
  const double vtg_ok = dir * 7;
  const double max_gearspace = 25;

  double dist_begin_l = get_dist_mm(-1);
  double dist_begin_r = get_dist_mm(1);

  double dl_last = get_dist_mm(-1), dr_last = get_dist_mm(1);
  double dl_now = dl_last, dr_now = dr_last;
  double delta_dist_l, delta_dist_r;

  LR_set_voltage(dir * 16, dir * 16);
  wait(20);
  double vtg_l = dir * 12, vtg_r = dir * 12;
  LR_set_voltage(vtg_l, vtg_r);

  timer t;
  while (1) {
    wait(30);
    dl_now = get_dist_mm(-1);
    dr_now = get_dist_mm(1);

    delta_dist_l = dl_now - dl_last;
    delta_dist_r = dr_now - dr_last;

    // COUT << "delta_dist_l: " << delta_dist_l << endl;
    // COUT << "delta_dist_r: " << delta_dist_r << endl;

    if (abs(delta_dist_l) <= 3 || abs(dist_begin_l - dl_now) > max_gearspace)
      vtg_l = vtg_ok;
    if (abs(delta_dist_r) <= 3 || abs(dist_begin_r - dr_now) > max_gearspace)
      vtg_r = vtg_ok;

    LR_set_voltage(vtg_l, vtg_r);

    if (vtg_l == vtg_ok && vtg_r == vtg_ok) {
      COUT << "0--gearspace removed abs: " << abs(dl_now - dist_begin_l) << ", "
           << abs(dr_now - dist_begin_r) << endl;
      break;
    }

    if (t.time() >= 150) {
      COUT << "0--gearspace more than: " << abs(dl_now - dist_begin_l) << ", "
           << abs(dr_now - dist_begin_r) << endl;
      break;
    }
    dl_last = dl_now;
    dr_last = dr_now;
  }
  return dl_now; // gear space none position
}

///////////////////////////////////////////////////////////////////////////////
/*
点动
*/
///////////////////////////////////////////////////////////////////////////////
void HChasis::point_move_relative(int dist_ralative, int timeout) {
  FT_set_voltage(sgn(dist_ralative) * 25, 0);
  double dist_aim_abs = get_dist_mm(-1) + dist_ralative;
  timer t;
  while (1) {
    if (sgn(dist_ralative) == 1) // froward
      if (get_dist_mm(-1) > dist_aim_abs - 0.25) {
        // COUT << "pmove: forward ok: " << get_dist_mm(-1) << endl;
        break;
      }
    if (sgn(dist_ralative) == -1) // backward
      if (get_dist_mm(-1) < dist_aim_abs + 0.25) {
        // COUT << "pmove: backward ok: " << get_dist_mm(-1) << endl;
        break;
      }

    if (t.time() > timeout) {
      // COUT << "pmve: timeout\n";
      break;
    }
    wait(2);
  }

  stop(brakeType::brake);
}
///////////////////////////////////////////////////////////////////////////////
/*
启动函数
*/
///////////////////////////////////////////////////////////////////////////////
void HChasis::forward_pid_adjust_dist_relative(
    double aim_dist_mm, double max_pwr, int timeout, bool waitForCompletion,
    bool b_remove_gearspace, bool b_reset_enc, double max_adjust_pwr) {

  //等待前一個线程结束
  while (forward_pid_adjust_running)
    wait(5);

  arg_forward_pid_adjust_dist_relative = {
      false,       aim_dist_mm,       max_pwr,
      timeout,     waitForCompletion, b_remove_gearspace,
      b_reset_enc, max_adjust_pwr};

  forward_pid_adjust_force_stop = false;

  //建立线程
  thread t(forward_pid_adjust_dist_relative_data_cb_static, this);

  //等待启动
  while (!forward_pid_adjust_running)
    wait(5);

  //等待完成
  if (waitForCompletion) {
    while (forward_pid_adjust_running && !forward_pid_adjust_force_stop)
      wait(10);
  }
}
///////////////////////////////////////////////////////////////////////////////
/*
线程执行函数
*/
double get_gyro(); //获取陀螺仪
///////////////////////////////////////////////////////////////////////////////
void HChasis::forward_pid_adjust_dist_relative_data_cb_static(void *p_this) {

  HChasis *p_this_chasis = (HChasis *)p_this;
  p_this_chasis->forward_pid_adjust_running = true;

  //建立局部变量，供本线程使用
  double aim_dist_mm =
      p_this_chasis->arg_forward_pid_adjust_dist_relative.aim_dist_mm;
  double max_pwr = p_this_chasis->arg_forward_pid_adjust_dist_relative.max_pwr;
  int timeout = p_this_chasis->arg_forward_pid_adjust_dist_relative.timeout;
  if (timeout == 0)
    timeout = 1000000; // long time enough

  bool b_remove_gearspace =
      p_this_chasis->arg_forward_pid_adjust_dist_relative.b_remove_gearspace;
  double max_adjust_pwr =
      p_this_chasis->arg_forward_pid_adjust_dist_relative.max_adjust_pwr;
  ////////////////////////////////////////////////////////////////////////////////////
  simple_cout << endl;
  simple_cout << "--------------------forward_pid_adjust_dist_relative: "
              << aim_dist_mm << " maxpwr: " << max_pwr
              << " timeout: " << timeout << endl;
  COUT << "**** starting position: " << p_this_chasis->get_dist_mm(-1) << endl;

  timer t;
  timer t_run;
  int begin_dir = sgn(aim_dist_mm);
  double begin_position;
  double aim_position_mm;

#define dist_abs_left abs(aim_position_mm - p_this_chasis->get_dist_mm(-1))

  double spd_abs = 20;

  // 1: remove gearspace
  if (b_remove_gearspace) {
    begin_position = p_this_chasis->remove_gearspace(begin_dir);
    if (p_this_chasis->arg_forward_pid_adjust_dist_relative.b_reset_enc) {
      p_this_chasis->reset_enc();
      begin_position = 0;
    }
    COUT << "    gearspace removed position: " << begin_position << endl;
  } else {
    if (p_this_chasis->arg_forward_pid_adjust_dist_relative.b_reset_enc)
      p_this_chasis->reset_enc();
    begin_position = p_this_chasis->get_dist_mm(-1);
  }
  // set current position as new reference point after removing gearspace
  const double braking_gearspace = 30;
  if (abs(aim_dist_mm) < 2 * braking_gearspace)
    return;
  // final aim position
  aim_position_mm =
      begin_position + aim_dist_mm - begin_dir * braking_gearspace;
  simple_cout << "    actual aim position: " << aim_position_mm << endl;

  double dist_all_abs = abs(aim_position_mm - begin_position);
  pid_ctrl pid_distance(0.15, 0.05, 0, 20, aim_position_mm, 5, 35, max_pwr);
  pid_ctrl pid_gyro_adjust(3.5, 0.02, 0, 20, get_gyro(), 0.15, 5, 20); //

  double turn_adjust = 0;
  double adjust = 0;
  double output_forward = 0;
  double dist_err;

  // 2: speeding
  spd_abs = 20;
  while (!p_this_chasis->forward_pid_adjust_force_stop &&
         t_run.time() < timeout && dist_abs_left > 60 &&
         dist_abs_left > dist_all_abs * 0.7) {
    turn_adjust = pid_gyro_adjust.pid_output(get_gyro());

    p_this_chasis->FT_set_voltage(begin_dir * spd_abs, turn_adjust);

    if (spd_abs > max_pwr - max_adjust_pwr) {
      spd_abs = max_pwr - max_adjust_pwr;
      break; // speeding ok break
    } else
      spd_abs += 1.5;
    wait(5);
  }
  COUT << "1--speeding end dist: " << p_this_chasis->get_dist_mm(-1) << endl;
  COUT << "   speeding end gyro: " << get_gyro() << endl;

  // 3: even speed close to aim
  while (!p_this_chasis->forward_pid_adjust_force_stop &&
         t_run.time() < timeout && dist_abs_left > 70 &&
         dist_abs_left > dist_all_abs * 0.05) {

    // forward
    dist_err = aim_position_mm - p_this_chasis->get_dist_mm(-1);
    // over run
    if (sgn(begin_dir) != sgn(dist_err))
      break;

    output_forward = pid_distance.pid_output_need_err(dist_err);

    turn_adjust = pid_gyro_adjust.pid_output(get_gyro());
    adjust = turn_adjust;

    max_limit(adjust, max_adjust_pwr);

    // output all
    max_limit(output_forward, max_pwr - abs(adjust));
    p_this_chasis->FT_set_voltage(output_forward, adjust);

    wait(2);
  }
  COUT << "2--even speed end dist: " << p_this_chasis->get_dist_mm(-1) << endl;
  COUT << "   even speed end gyro: " << get_gyro() << endl;

  // 4: slow down
  spd_abs = abs(output_forward);
  while (!p_this_chasis->forward_pid_adjust_force_stop &&
         t_run.time() < timeout && dist_abs_left > 15 &&
         dist_abs_left > dist_all_abs * 0.02) {
    // turn_adjust = pid_gyro_adjust.pid_output(get_gyro());

    turn_adjust = 0;

    p_this_chasis->FT_set_voltage(begin_dir * spd_abs, turn_adjust);

    // over run
    dist_err = aim_position_mm - p_this_chasis->get_dist_mm(-1);
    if (sgn(begin_dir) != sgn(dist_err))
      break;

    spd_abs--;
    if (spd_abs < 15) {
      if (dist_abs_left < 20) {
        if (spd_abs <= 2) {
          spd_abs = 2;
          break;
        } else
          spd_abs -= 2;
      } else // left too much
        spd_abs = 15;
    }
    wait(2);
  }
  COUT << "3--slow down end dist: " << p_this_chasis->get_dist_mm(-1) << endl;
  COUT << "   slow down end gyro: " << get_gyro() << endl;

  // 5: point move to aim
  int pk = 0;
  bool pmove_err_ok = false;
  while (!p_this_chasis->forward_pid_adjust_force_stop &&
         t_run.time() < timeout) {
    dist_err = aim_position_mm - p_this_chasis->get_dist_mm(-1);

    if (abs(dist_err) > 10)
      p_this_chasis->point_move_relative(5 * sgn(dist_err));
    else
      p_this_chasis->point_move_relative(3 * sgn(dist_err));
    // COUT << "pmove  err: " << dist_err << endl;
    if (abs(dist_err) < 5) {
      pmove_err_ok = true;
      break;
    }
    // // over run
    // dist_err = aim_position_mm - get_dist_mm(-1);
    // if (sgn(begin_dir) != sgn(dist_err))
    //   break;
    pk++;
    wait(2);
  }
  if (!p_this_chasis->forward_pid_adjust_force_stop) {
    COUT << "4--pmove end dist: " << p_this_chasis->get_dist_mm(-1) << endl;
    COUT << "   pmove end gyro: " << get_gyro() << endl;
    COUT << "   pmove end left_dist: " << dist_abs_left << endl;
    COUT << "   pmove pk: " << pk << endl;
    if (pmove_err_ok)
      COUT << "   pmove ok err: " << dist_err << endl;

    if (t_run.time() > timeout) {
      simple_cout << "    **** final---timeout!!!" << endl;
    } else if (pmove_err_ok) {
      simple_cout << "    **** ---OK  time: " << t_run.time() << endl;
    }
  } else
    simple_cout << "    **** ---force stop!!!  time: " << t_run.time() << endl;

  // 6: stop
  p_this_chasis->stop(brakeType::brake);
  p_this_chasis->arg_forward_pid_adjust_dist_relative.run_done = true;
  p_this_chasis->forward_pid_adjust_running = false;
}
