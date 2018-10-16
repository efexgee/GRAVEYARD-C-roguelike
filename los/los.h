#ifndef INC_LOS_H
#define INC_LOS_H

#include <math.h>
#include <assert.h>
#include <stdlib.h>

void step_towards(int *x, int *y, int target_x, int target_y, int diagonal);
void update(int *stepper, int *bumper, float slope, int step, int bump, float *acc_err);
void next_square(int *x, int *y, int x_direction, float slope, float *acc_err);

#endif
