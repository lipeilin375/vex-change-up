#pragma once

#include "../VirtualButton/VirtualButton.h"
#include "vex.h"


// screen size: 480x240
#define LCD_MAX_X 480
#define LCD_MAX_Y 240

void buttons_init();
void buttons_show_up();
void screen_display_choose();

void clear_display_area();
void display_manager_thread_routine();
/////////////////////////////////////////
enum class Display_Page {
  sensor_info = 0,
  controller_axis_info,
  port_info,
  max_info,
};

extern Display_Page cur_page;
extern Display_Page last_page;
extern bool display_cleared;

void page_up();
void page_down();

void disp_page_sensors();
void disp_page_ports();
void disp_page_axis();

extern BtnRectangle *auto_side_role_btn[4];
extern BtnRectangle *vb_route;
extern BtnRectangle *vb_auto;
extern BtnRectangle *vb_gyro_rec;