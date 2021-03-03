#include "BtnShape.h"
/************************************************
整型转换成string
*/
#include <stdio.h>
string i2str(int k) {
  char buf[15];
  sprintf(buf, "%d", k);
  return string(buf);
}

/************************************************
静态类变量初始化
*/
int BtnShape::btn_count = 0;
bool BtnShape::vec_ptr_all_btns_changing = false;

//老版本直接定义类变量，但是会在main函数运行之前调用vector的构造函数将其清空
// vector<BtnShape *> BtnShape::vec_ptr_all_btns;

/************************************************
获取vec_ptr_all_btns的引用
*/
vector<BtnShape *> &BtnShape::get_vec_ptr_all_btns() {
  static vector<BtnShape *> vec_ptr_all_btns;
  // vec_ptr_all_btns为所有实例的指针向量,定义为成数内部static变量,在构造函数中会调用此函数
  //保证在实例化任意一个BtnShape之前先初始化此变量,进而可以定义全局BtnShape类变量
  //而不会在main函数开始运行之前调用vector的构造函数将已经push_back的全局BtnShape清空
  //导致其保存的callback中读取不到
  return vec_ptr_all_btns;
}

/************************************************
构造函数
*/
BtnShape::BtnShape(color c_b, color c_f, std::string s, fontType ft,
                   color c_lable)
    : c_border(c_b), c_fill(c_f), lable(s), font(ft), lable_color(c_lable),
      ent(this) {
  while (!BtnEvent::thread_resting) //等待线程休息时再添加
    wait(1);

  lable_on_create = lable; //保存创建时的标签
  btn_count++;
  vec_ptr_all_btns_changing = true;
  get_vec_ptr_all_btns().push_back(this); //保存本实例指针
  vec_ptr_all_btns_changing = false;
}

/************************************************
析构函数
*/
BtnShape::~BtnShape() {
  //不能在构造（析构）函数中调用纯虚函数，因为子类还不存在（已经消失）
  // hide();

  while (!BtnEvent::thread_resting) { //等待线程休息时再删除
    wait(1);
  }

  vec_ptr_all_btns_changing = true;
  //从向量中删除自己
  auto iter = get_vec_ptr_all_btns().begin();
  while (iter != get_vec_ptr_all_btns().end()) {
    if (*iter == this) {
      iter = get_vec_ptr_all_btns().erase(iter);
      break; //向量中每个按钮指针值保存一个，没有重复，所以break，不需要继续搜索
    } else
      iter++;
  }
  vec_ptr_all_btns_changing = false;
}

/************************************************
深度复制,包括回调函数
*/
void BtnShape::copy(const BtnShape &s) {
  c_border = s.c_border;
  c_fill = s.c_fill;
  lable = s.lable;
  font = s.font;
  lable_color = s.lable_color;

  on_brighter = s.on_brighter;
  visible = true;
  id = s.id; //按钮id编号

  xc_s = s.xc_s;
  yc_s = s.yc_s; //中心点坐标

  while (!BtnEvent::thread_resting) //等待线程休息时再添加
    wait(1);

  lable_on_create = lable; //保存创建时的标签
  btn_count++;
  vec_ptr_all_btns_changing = true;
  get_vec_ptr_all_btns().push_back(this); //保存本实例指针
  vec_ptr_all_btns_changing = false;
}

/************************************************
拷贝构造函数,深度拷贝,包括图形参数和保存实例指针，但是不包括回调函数
*/
BtnShape::BtnShape(const BtnShape &s) : ent(this) { copy(s); }

/************************************************
赋值构造函数,深度赋值,包括回调函数
*/
const BtnShape &BtnShape::operator=(const BtnShape &s) {
  copy(s);
  //同时赋值回调函数
  register_press_callback(s.ent.pressed_cb);
  register_release_callback(s.ent.released_cb);
  return *this;
}

/************************************************
设置标签
*/
void BtnShape::set_lable(string l) {
  print_lable(screen_get_background_color());
  lable = l;
  print_lable(lable_color);
}

/************************************************
相对移动
*/
void BtnShape::move_for(int dx, int dy, bool hide_former) {
  if (hide_former)
    hide();

  //更新中心坐标
  xc_s += dx;
  yc_s += dy;

  screen_store_pen_color();
  screen_set_pen_color(c_border);

  if (on_brighter)
    draw_shape(c_fill); //派生类的画形状函数,并移动
  else
    draw_shape(get_darker_color()); //派生类的画形状函数,并移动

  screen_restore_pen_color();

  print_lable(lable_color);
  visible = true;
}

void BtnShape::move_to(int x_new, int y_new, bool hide_former) {
  if (hide_former)
    hide();

  //更新中心坐标
  xc_s = x_new;
  yc_s = y_new;

  screen_store_pen_color();
  screen_set_pen_color(c_border);

  if (on_brighter)
    draw_shape(c_fill); //派生类的画形状函数,并移动
  else
    draw_shape(get_darker_color()); //派生类的画形状函数,并移动

  screen_restore_pen_color();

  print_lable(lable_color);
  visible = true;
}

/************************************************
打印按钮标签函数
*/
void BtnShape::print_lable(color c, bool transparent) {
  lable_x = xc_s - screen_get_font_width(font) * lable.size() / 2.0;
  lable_y = yc_s + screen_get_font_height(font) / 2.0;

  screen_store_pen_color();
  screen_store_pen_font();

  screen_set_pen_color(c);
  screen_set_pen_font(font);

  Brain.Screen.printAt(lable_x, lable_y, !transparent, "%s",
                       lable.c_str()); // write lable
  screen_restore_pen_color();
  screen_restore_pen_font();
}

/************************************************
3个r、g、b分量合成总的rgb值
*/
uint32_t BtnShape::rgb_compose(uint8_t v_r, uint8_t v_g, uint8_t v_b) {
  uint32_t rgb = (v_r << 16) + (v_g << 8) + (v_b << 0);
  return rgb;
}

/************************************************
rgb值分解到3个变量上
*/
void BtnShape::rgb_resolve(uint32_t rgb_v, uint8_t &rr, uint8_t &gg,
                           uint8_t &bb) {
  rr = (rgb_v >> 16) & 0xff;
  gg = (rgb_v >> 8) & 0xff;
  bb = (rgb_v >> 0) & 0xff;
}

/************************************************
获取暗色
*/
color BtnShape::get_darker_color(double darker_rate) {
  uint8_t r, g, b;
  rgb_resolve(c_fill.rgb(), r, g, b); //分解到r,g,b三个变量上

  r *= darker_rate;
  g *= darker_rate;
  b *= darker_rate;
  uint32_t new_rgb = rgb_compose(r, g, b);
  return color(new_rgb);
}

/************************************************
按钮变灰
*/
void BtnShape::darker(bool check_current_status, double darker_rate) {

  if (check_current_status) {
    if (!visible)
      return;
    if (visible && !on_brighter)
      return;
  }

  screen_store_pen_color();
  screen_set_pen_color(c_border);

  draw_shape(get_darker_color(darker_rate)); //派生类的画形状函数

  screen_restore_pen_color();

  print_lable(lable_color);
  on_brighter = false;
  visible = true;
}

/************************************************
按钮变亮
*/
void BtnShape::brighter(bool check_current_status) {

  if (check_current_status) {
    if (!visible)
      return;
    if (visible && on_brighter)
      return;
  }

  screen_store_pen_color();
  screen_set_pen_color(c_border);

  draw_shape(c_fill); //派生类的画形状函数

  screen_restore_pen_color();

  print_lable(lable_color);
  on_brighter = true;
  visible = true;
}

/************************************************
获取按钮是否被按下
*/
bool BtnShape::pressing() {
  if (!Brain.Screen.pressing())
    return false;
  if (!visible)
    return false;

  return in_shape(Brain.Screen.xPosition(), Brain.Screen.yPosition());
}

/************************************************
按钮显示
*/
void BtnShape::show_up(bool bright) {
  visible = true;
  if (bright)
    brighter(false);
  else
    darker(false);
}

/************************************************
按钮隐藏
*/
void BtnShape::hide() {
  visible = false;
  print_lable(screen_get_background_color(), false);

  screen_store_pen_color();
  screen_set_pen_color(screen_get_background_color());

  draw_shape(screen_get_background_color()); //派生类的画形状函数

  screen_store_pen_color();
}
