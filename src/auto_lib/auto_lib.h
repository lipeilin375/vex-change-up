#pragma once
#include "../_includes.h"

bool line_sensor_detected(line sensor, int threshold);

#define BALL_AT_CARRIER_ENTRY line_sensor_detected(carry_entry_detector, 63)
#define BALL_AT_INTAKE line_sensor_detected(intake_detector, 60)
#define BALL_AT_CARRIER_END line_sensor_detected(carry_end_detector, 65)
#define BALL_AT_SHOOTER line_sensor_detected(shoot_detector, 60)

void carry_load_ball();