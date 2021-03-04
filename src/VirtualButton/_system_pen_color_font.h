#pragma once

#include "v5_color.h"
#include "vex.h"

#define SCREEN_MAX_X 480
#define SCREEN_MAX_Y 240

class Screen_Data {
public:
  /*
  全局画笔颜色
  */
  color brainPenColor; //全局画笔颜色
  color colorPenTemp;

  /*
  全局填充颜色
  */
  color brainFillColor; //全局填充颜色
  color colorFillTemp;

  /*
  全局字体
  */
  fontType brainPenFont; //全局字体
  fontType fontTemp;

public:
  Screen_Data(color cp, color cf, fontType ft)
      : brainPenColor(cp), brainFillColor(cf), brainPenFont(ft) {}

  void init() {
    Brain.Screen.setPenColor(brainPenColor);
    Brain.Screen.setFillColor(brainFillColor);
    // Brain.Screen.clearScreen(brainFillColor);
    Brain.Screen.setFont(brainPenFont);
  }
};

extern Screen_Data sc;

//屏幕相关全局函数
color screen_get_background_color();
void screen_fill_background_color(color c);

void screen_store_color();   //保存颜色
void screen_restore_color(); //恢复颜色

void screen_set_pen_color(color c);
color screen_get_pen_color();
void screen_restore_pen_color(); //保存画笔颜色
void screen_store_pen_color();   //恢复画笔颜色

void screen_set_fill_color(color c);
color screen_get_fill_color();
void screen_restore_fill_color(); //恢复填充颜色
void screen_store_fill_color();   //保存填充颜色

void screen_set_pen_font(fontType ft);
fontType screen_get_pen_font();
void screen_restore_pen_font(); //恢复字体
void screen_store_pen_font();   //保存字体
int screen_get_font_width(fontType ft);
int screen_get_font_height(fontType ft);
void screen_clear(const color &clr = black);
