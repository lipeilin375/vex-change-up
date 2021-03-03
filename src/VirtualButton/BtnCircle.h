#pragma once

#include "BtnShape.h"

class BtnCircle : public BtnShape {
private:
  double c_x, c_y;
  double r;

public:
  ~BtnCircle() { hide(); }

  BtnCircle() {}
  BtnCircle(double xx, double yy, double rr, bool create_on_brighter, color c_b,
            color c_f, std::string s, fontType ft = fontType::mono15,
            color c_lable = white)
      : BtnShape(c_b, c_f, s, ft, c_lable), c_x(xx), c_y(yy), r(rr) {

    xc_s = xx; // shape 的x中心坐标
    yc_s = yy; // shape 的y中心坐标

    show_up(create_on_brighter); // shape 根据create_on_brighter显示出来
  }
  ////////////////////////////////////////////////
  bool in_shape(double x, double y) {
    if ((c_x - x) * (c_x - x) + (c_y - y) * (c_y - y) <= r * r)
      return true;
    else
      return false;
  }

  void draw_shape(color c) {
    //根据Shape的中心坐标和Circle的参数画图
    Brain.Screen.drawCircle(xc_s, yc_s, r, color(c));

    // xc_s、yc_s已经在外部更新，更新Circle的参数
    c_x = xc_s;
    c_y = yc_s;
  }
};