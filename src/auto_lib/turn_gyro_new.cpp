#include "../globals/globals.h"
#include "../pid/pid.h"

#define COUT_TURN                                                              \
  if (0)                                                                       \
  cout
#define simple_cout_2                                                          \
  if (0)                                                                       \
  cout
void turn_pid_adjust_agl_absolute_data_cb_static(void *arg);

double get_gyro() { return imu.rotation(); }
double get_gyro_rate() {
  return imu.gyroRate(axisType::zaxis, velocityUnits::dps);
}
void gyro_reset() { imu.resetRotation(); }

bool turn_pid_adjust_force_stop = false;
bool turn_pid_adjust_running = false;

struct Arg_turn_pid_adjust_agl_absolute {
  bool run_done; //状态标志，是否完成
  double aim_agl;
  double max_pwr;
  int timeout;
  bool waitForCompletion;
  double max_adjust_pwr;
} arg_turn_pid_adjust_agl_absolute;
//////////////////////////////////////////////////////////////////////////////
void turn_pid_adjust_stop() { turn_pid_adjust_force_stop = true; }
bool is_turn_pid_adjust_done() {
  return arg_turn_pid_adjust_agl_absolute.run_done;
}
///////////////////////////////////////////////////////////////////////////////
/*
点动
*/
///////////////////////////////////////////////////////////////////////////////
void turn_point_move_absolute(double agl_ralative, int timeout = 100) {
  chasis.FT_set_voltage(0, sgn(agl_ralative) * 17.5);
  double agl_aim_abs = get_gyro() + agl_ralative;
  timer t;
  while (1) {
    if (sgn(agl_ralative) == 1) // froward
      if (get_gyro() > agl_aim_abs - 0.08) {
        // COUT_TURN << "pmove: turn ok: " << get_gyro() << endl;
        break;
      }
    if (sgn(agl_ralative) == -1) // backward
      if (get_gyro() < agl_aim_abs + 0.08) {
        // COUT_TURN << "pmove: backward ok: " << get_gyro() << endl;
        break;
      }

    if (t.time() > timeout) {
      // COUT_TURN << "pmve: timeout\n";
      break;
    }
    wait(2);
  }

  chasis.stop(brakeType::brake);
}
///////////////////////////////////////////////////////////////////////////////
/*
启动函数
*/
///////////////////////////////////////////////////////////////////////////////
void turn_pid_adjust_agl_absolute(double aim_agl, double max_pwr, int timeout,
                                  bool waitForCompletion,
                                  double max_adjust_pwr) {

  //等待前一個线程结束
  while (turn_pid_adjust_running)
    wait(5);

  arg_turn_pid_adjust_agl_absolute = {
      false, aim_agl, max_pwr, timeout, waitForCompletion, max_adjust_pwr};

  turn_pid_adjust_force_stop = false;

  //建立线程
  thread t(turn_pid_adjust_agl_absolute_data_cb_static,
           &arg_turn_pid_adjust_agl_absolute);

  //等待启动
  while (!turn_pid_adjust_running)
    wait(5);

  //等待完成
  if (waitForCompletion) {
    while (turn_pid_adjust_running && !turn_pid_adjust_force_stop)
      wait(10);
  }
}
///////////////////////////////////////////////////////////////////////////////
/*
线程执行函数
*/
double get_gyro(); //获取陀螺仪
double get_gyro_rate();
///////////////////////////////////////////////////////////////////////////////
void turn_pid_adjust_agl_absolute_data_cb_static(void *arg) {

  turn_pid_adjust_running = true;

  //建立局部变量，供本线程使用
  double aim_agl = arg_turn_pid_adjust_agl_absolute.aim_agl;
  double max_pwr = arg_turn_pid_adjust_agl_absolute.max_pwr;
  int timeout = arg_turn_pid_adjust_agl_absolute.timeout;
  if (timeout == 0)
    timeout = 1000000; // long time enough

  double max_adjust_pwr = arg_turn_pid_adjust_agl_absolute.max_adjust_pwr;
  ////////////////////////////////////////////////////////////////////////////////////
  simple_cout_2 << endl;
  simple_cout_2 << "--------------------turn_pid_adjust_agl_absolute: "
                << aim_agl << " maxpwr: " << max_pwr << " timeout: " << timeout
                << endl;
  COUT_TURN << "**** starting angle: " << get_gyro() << endl;

  timer t;
  timer t_run;
  int begin_dir = sgn(aim_agl - get_gyro());

  double begin_angle;
  double aim_angle = aim_agl;

#define agl_abs_left abs(aim_angle - get_gyro())

  double spd_abs = 20;

  begin_angle = get_gyro();

  double agl_all_abs = abs(aim_angle - begin_angle);
  pid_ctrl pid_angle(1.2, 0.05, 0, 20, aim_angle, 0.3, 5, max_pwr);
  pid_ctrl pid_wheels_speed_adjust(0.25, 0.002, 0, 10, 0, 2, 5, max_adjust_pwr);

  double output_wheel_speed_adjust = 0;
  double output_turn = 0;
  double agl_err;

  // 2: even speed close to aim
  while (!turn_pid_adjust_force_stop && t_run.time() < timeout &&
         agl_abs_left > 5 && agl_abs_left > agl_all_abs * 0.05) {

    // turn
    agl_err = aim_angle - get_gyro();
    // over run
    if (sgn(begin_dir) != sgn(agl_err))
      break;

    output_turn = pid_angle.pid_output_need_err(agl_err);
    if (abs(output_turn) < 15)
      output_turn = sgn(output_turn) * 15;

    output_wheel_speed_adjust = pid_wheels_speed_adjust.pid_output_need_err(
        chasis.get_velocity(-1) + chasis.get_velocity(1));
    // output all
    chasis.LR_set_voltage(output_turn - output_wheel_speed_adjust,
                          -output_turn - output_wheel_speed_adjust);
    wait(2);
  }
  COUT_TURN << "2--even speed end agl: " << get_gyro() << endl;

  // 3: slow down
  spd_abs = abs(output_turn);
  while (!turn_pid_adjust_force_stop && t_run.time() < timeout &&
         agl_abs_left > 1.5 && agl_abs_left > agl_all_abs * 0.02) {
    output_wheel_speed_adjust = pid_wheels_speed_adjust.pid_output_need_err(
        chasis.get_velocity(-1) + chasis.get_velocity(1));

    chasis.LR_set_voltage(begin_dir * spd_abs - output_wheel_speed_adjust,
                          -begin_dir * spd_abs - output_wheel_speed_adjust);

    // over run
    agl_err = aim_angle - get_gyro();
    if (sgn(begin_dir) != sgn(agl_err))
      break;

    spd_abs--;
    if (spd_abs < 15) {
      if (agl_abs_left < 3) {
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
  COUT_TURN << "3--slow down end agl: " << get_gyro() << endl;
  COUT_TURN << "   slow down end gyro: " << get_gyro() << endl;

  // 4: point move to aim
  int pk = 0;
  bool pmove_err_ok = false;
  while (!turn_pid_adjust_force_stop && t_run.time() < timeout) {
    agl_err = aim_angle - get_gyro();

    if (abs(agl_err) > 3)
      turn_point_move_absolute(0.1 * sgn(agl_err));
    else
      turn_point_move_absolute(0.2 * sgn(agl_err));
    if (abs(agl_err) < 0.2 && abs(get_gyro_rate()) < 25) {
      pmove_err_ok = true;
      break;
    }
    pk++;
    wait(2);
  }
  if (!turn_pid_adjust_force_stop) {
    COUT_TURN << "4--pmove end agl: " << get_gyro() << endl;
    COUT_TURN << "   pmove end gyro: " << get_gyro() << endl;
    COUT_TURN << "   pmove end left_agl: " << agl_abs_left << endl;
    COUT_TURN << "   pmove pk: " << pk << endl;
    if (pmove_err_ok)
      COUT_TURN << "   pmove ok err: " << agl_err << endl;

    if (t_run.time() > timeout) {
      simple_cout_2 << "    **** final---timeout!!!" << endl;
    } else if (pmove_err_ok) {
      simple_cout_2 << "    **** ---OK  time: " << t_run.time() << endl;
    }
  } else
    simple_cout_2 << "    **** ---force turn stop!!!  time: " << t_run.time()
                  << endl;

  // 6: chasis.stop
  chasis.stop(brakeType::brake);
  arg_turn_pid_adjust_agl_absolute.run_done = true;
  turn_pid_adjust_running = false;
}
