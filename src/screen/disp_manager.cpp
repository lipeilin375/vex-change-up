#include "../_includes.h"
#include "screen.h"

Display_Page cur_page = Display_Page::sensor_info;
Display_Page last_page = cur_page;
bool display_cleared = true;

void clear_display_area() {
  screen_store_pen_font();
  screen_set_pen_font(fontType::mono20);

  for (int line_y = 0; line_y <= LCD_MAX_Y; line_y += 10)
    Print(0, line_y, "                         ");
  display_cleared = true;
  screen_restore_pen_font();
}

void page_up() {
  int p = (int)cur_page;
  p++;
  p %= (int)Display_Page::max_info;
  cur_page = (Display_Page)p;
}

void page_down() {
  int p = (int)cur_page;
  p--;
  if (p == -1)
    p = (int)Display_Page::max_info - 1;
  cur_page = (Display_Page)p;
}

void display_manager_thread_routine() {
  buttons_init();
  while (1) {
    // virtual btn start auto acccording to auto setting
    if (vb_auto->pressing()) {
      timer t;
      while (vb_auto->pressing()) {
        if (t.time() > 1000) {
          vb_auto->brighter();
          bool cancelled = false;
          while (Brain.Screen.pressing()) {
            if (!vb_auto->pressing()) {
              vb_auto->darker();
              cancelled = true;
            }
            wait(5);
          }
          if (!cancelled)
            run_auto_background();
          vb_auto->darker();
        }
        wait(5);
      }
    }

    // imu calibrate
    if (vb_gyro_rec->pressing()) {
      timer t;
      while (vb_gyro_rec->pressing()) {
        if (t.time() > 1000) {
          vb_gyro_rec->brighter();
          bool cancelled = false;
          while (Brain.Screen.pressing()) {
            if (!vb_gyro_rec->pressing()) {
              vb_gyro_rec->darker();
              cancelled = true;
            }
            wait(5);
          }
          if (!cancelled) {
            imu.calibrate();
            while_wait(imu.isCalibrating());
          }
          vb_gyro_rec->darker();
        }
        wait(5);
      }
    }

    if (cur_page != last_page) {
      clear_display_area();
      last_page = cur_page;
    }

    if (cur_page == Display_Page::sensor_info)
      disp_page_sensors();
    if (cur_page == Display_Page::port_info)
      disp_page_ports();
    if (cur_page == Display_Page::controller_axis_info)
      disp_page_axis();

    wait(100);
    // pause updatint when autonomous_running
    if (autonomous_running) {
      while (autonomous_running)
        wait(500);
      wait(1000);
      screen_set_pen_font(fontType::mono20);
      screen_clear();
      buttons_show_up();
    }
  }
}