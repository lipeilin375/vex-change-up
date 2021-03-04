#pragma once

#define VirtualButton_Version 20200905

#include "_system_pen_color_font.h"
#include "vex.h"
#include <string>
#include <vector>
string i2str(int k);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "BtnEvent.h"

class BtnShape {
  //需要子类实现的函数
public:
  virtual bool in_shape(double x, double y) = 0;
  virtual void draw_shape(color c_fill) = 0;

  //内部变量
private:
  color c_border, c_fill;
  std::string lable = "";
  string lable_on_create; //保存创建时的标签

  fontType font = fontType::mono15; //标签字体
  color lable_color = color::white;
  int lable_x, lable_y; //自动计算
  bool on_brighter = true;

  bool visible = false;
  int id = -1; //按钮id编号

public:
  int xc_s, yc_s; //中心点坐标

  //////////////////////////////////////////////////////////////////////////////////
private:
  void copy(const BtnShape &s);

public:
  BtnShape() : ent(this) {} //必须初始化ent
  BtnShape(color c_b, color c_f, std::string s, fontType ft = fontType::mono15,
           color c_lable = color(ClrWhite));
  ~BtnShape();
  BtnShape(const BtnShape &s);                  //拷贝构造函数
  const BtnShape &operator=(const BtnShape &s); //赋值构造函数
  void set_id(int x) { id = x; }                //设置按钮编号
  int get_id() { return id; }                   //获取按钮编号

  string get_lable() { return lable; }                      //获取标签
  string get_lable_on_created() { return lable_on_create; } //获取标签

  void set_lable(string l); //设置标签
  void set_color_fill(color c) {
    c_fill = c;
    show_up();
  } //设置填充色

  void set_color_border(color c) {
    c_border = c;
    show_up();
  } //设置边界色

  int get_xc_s() { return xc_s; } //获取中心坐标
  int get_yc_s() { return yc_s; } //获取中心坐标

  void move_for(int dx, int dy, bool hide_former = true);      //相对移动
  void move_to(int x_new, int y_new, bool hide_former = true); //绝对移动

private:
  uint32_t rgb_compose(uint8_t v_r, uint8_t v_g, uint8_t v_b);
  void rgb_resolve(uint32_t rgb_v, uint8_t &rr, uint8_t &gg, uint8_t &bb);
  color get_darker_color(double darker_rate = 0.25);

public:
  void print_lable(color c, bool transparent = true);
  void darker(bool check_current_status = true, double darker_rate = 0.25);
  void brighter(bool check_current_status = true);

  bool pressing();
  ////////////////////
  void show_up(bool bright);
  void show_up() { show_up(on_brighter); } //默认显示当前状态
  void hide();

  /////////////////////////////////////////////////////////////////////////////////////////
public:
  static int btn_count;
  static bool vec_ptr_all_btns_changing;
  // static vector<BtnShape *> vec_ptr_all_btns; //所有实例的指针向量
  static vector<BtnShape *> &get_vec_ptr_all_btns(); //获取所有实例的指针向量

public:
  BtnEvent ent; // BtnEvent对象，用以保存按钮的状态参数

  //注册按下回调函数
  void register_press_callback(pBtnCallback cb) { ent.save_pressed_cb(cb); }

  //注册松开回调函数
  void register_release_callback(pBtnCallback cb) { ent.save_released_cb(cb); };

  void cancel_press_callback() { ent.cancel_pressed_cb(); }
  void cancel_release_callback() { ent.cancel_released_cb(); }
};
