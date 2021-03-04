#pragma once

#include "VirtualButton/VirtualButton.h"
#include "auto/auto.h"
#include "globals/globals.h"
#include "screen/screen.h"
#include "vex.h"

void auto_thread_routine();
void controller_print_team_number();
void run_auto_background();
void special_key_handle();

class Clock {
public:
  unsigned int get_msec() { return timer::system(); }
  unsigned int get_sec() { return timer::system() / 1000 % 60; }
  unsigned int get_min() { return timer::system() / 1000 / 60; }
};

///////////////////////////////////////////////////////////////////////////////////////////////
