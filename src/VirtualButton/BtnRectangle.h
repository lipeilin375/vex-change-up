#pragma once

#include "BtnShape.h"

class BtnRectangle : public BtnShape {
private:
  double x_left_top, y_left_top;
  double w, h; //宽度、高度

public:
  ~BtnRectangle() { hide(); }
  BtnRectangle() {}
  BtnRectangle(double xx, double yy, double ww, double hh,
               bool create_on_brighter, color c_b, color c_f, std::string s,
               fontType ft = fontType::mono15, color c_lable = white)
      : BtnShape(c_b, c_f, s, ft, c_lable) {

    // Rectangle的参数
    w = ww;
    h = hh;
    x_left_top = xx; // Rectangle的左上角x坐标
    y_left_top = yy; // Rectangle的左上角y坐标

    // Shape的参数
    xc_s = x_left_top + w / 2; // shape 的x中心坐标
    yc_s = y_left_top + h / 2; // shape 的y中心坐标

    show_up(create_on_brighter); // shape 根据create_on_brighter显示出来
  }
  ////////////////////////////////////////////////
  bool in_shape(double x, double y) {
    if ((x >= x_left_top) && (x <= x_left_top + w) && (y >= y_left_top) &&
        (y <= y_left_top + h))
      return true;
    else
      return false;
  }

  void draw_shape(color c_fill) {
    //根据Shape的中心坐标和Rectangle的参数画图
    Brain.Screen.drawRectangle(xc_s - w / 2, yc_s - h / 2, w, h, c_fill);

    // xc_s、yc_s已经在外部更新，更新Rectangle的参数
    x_left_top = xc_s - w / 2;
    y_left_top = yc_s - h / 2;
  }
};