#include "_system_pen_color_font.h "
#include "vex.h"

//全局屏幕颜色字体数据
Screen_Data sc(color(ClrWhite), color(ClrBlack), fontType::mono20);

void screen_clear(const color &clr) { Brain.Screen.clearScreen(clr); }
//////////////////////////////////////////////////////
void screen_set_fill_color(color c) {
  sc.brainFillColor = c;
  Brain.Screen.setFillColor(c);
}
color screen_get_fill_color() { return sc.brainFillColor; }
void screen_restore_fill_color() {
  screen_set_fill_color(sc.colorFillTemp);
} //恢复填充颜色

void screen_store_fill_color() {
  sc.colorFillTemp = sc.brainFillColor;
} //保存填充颜色

color screen_get_background_color() { return sc.brainFillColor; }
void screen_fill_background_color(color c) {
  sc.brainFillColor = c;
  Brain.Screen.clearScreen(c);
}
////////////////

void screen_set_pen_color(color c) {
  sc.brainPenColor = c;
  Brain.Screen.setPenColor(c);
}
color screen_get_pen_color() { return sc.brainPenColor; }
void screen_restore_pen_color() {
  screen_set_pen_color(sc.colorPenTemp);
} //恢复画笔颜色
void screen_store_pen_color() {
  sc.colorPenTemp = sc.brainPenColor;
} //保存画笔颜色
///////////////////////////////////
void screen_store_color() {
  screen_store_pen_color();
  screen_store_fill_color();
} //保存颜色
void screen_restore_color() {
  screen_restore_pen_color();
  screen_restore_fill_color();
} //恢复颜色

///////////////////////////////////////////////////////////////////////////////////////////////////
void screen_set_pen_font(fontType ft) {
  sc.brainPenFont = ft;
  Brain.Screen.setFont(ft);
}

fontType screen_get_pen_font() { return sc.brainPenFont; }
void screen_restore_pen_font() {
  screen_set_pen_font(sc.fontTemp);
} //恢复画笔颜色
void screen_store_pen_font() { sc.fontTemp = sc.brainPenFont; } //保存画笔颜色

//库文件定义
// enum class fontType {
//   mono20 = 0,
//   mono30,
//   mono40,
//   mono60,
//   prop20,
//   prop30,
//   prop40,
//   prop60,
//   mono15,
//   mono12,
//   cjk16
// };

int font_width_height_data[11][2] = {
    //{height,width}
    {14, 10}, // mono20 :
    {20, 15}, // mono30
    {26, 20}, // mono40
    {39, 28}, // mono60
    {15, 10}, // prop20
    {22, 15}, // prop30
    {29, 19}, // prop40
    {42, 28}, // prop60
    {10, 8},  // mono15
    {9, 6},   // mono12
    {14, 9},  // cjk16
};

int screen_get_font_width(fontType ft) {
  return font_width_height_data[(int)ft][1];
}

int screen_get_font_height(fontType ft) {
  return font_width_height_data[(int)ft][0];
}
