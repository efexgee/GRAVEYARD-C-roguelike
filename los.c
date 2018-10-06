#include "los.h"
#include <stdio.h>

void next_square(int *x, int *y, int x_step, float y_step, float *acc_err) {
    // Change a position to the next position along an angle (sort of)
    // This requires y_step and acc_err to be passed in so this really can't
    // exist on its own. That feels weird.

    fprintf(stderr, "%s((%d, %d), x_step=%d, y_step=%.2f, acc_err=%.2f)\n", "next_square", *x, *y, x_step, y_step, *acc_err);

    // use abs() instead?
    if (y_step == INFINITY || y_step == -INFINITY) {
        // Just moving vertically.
        fprintf(stderr, "%s: y_step is infinity\n", "next_square");
        *y += y_step < 0 ? -1 : 1;
        return;
    }


    if (y_step == 0) {
        // Just moving horizontally
        fprintf(stderr, "%s: y_step is zero\n", "next_square");
        *x += x_step;
        return;
    }

    int y_jump = 1;
    int x_jump = 1;

    if (y_step < 0) {
        y_jump = -1;
    }

    if (x_step < 0) {
        x_jump = -1;
    }

    if (fabs(y_step) > 1) {
        *y += y_jump;

        float x_slope = 1 / y_step;
        float ideal_x = *x + *acc_err + x_slope; 

        fprintf(stderr, "%s: steep slope x_jump=%d y_jump=%d: x_slope=%.2f ideal_x=%.2f y=%d\n", "next_square", x_jump, y_jump, x_slope, ideal_x, *y);

        if (fabs(ideal_x - *x) < 0.5) {
	        *acc_err = ideal_x - *x;
        } else if (fabs(ideal_x - *x) > 0.5) {
            *x += x_jump;
            *acc_err = ideal_x - *x;
        }
    } else {
        *x += x_step;
        float ideal_y = *y + *acc_err + y_step;

        fprintf(stderr, "%s: steep slope x_jump=%d y_jump=%d: ideal_y=%.2f x=%d\n", "next_square", x_jump, y_jump, ideal_y, *x);
        if (fabs(ideal_y - *y) < 0.5) {
	        *acc_err = ideal_y - *y;
        } else if (fabs(ideal_y - *y) > 0.5) {
            *y += y_jump;
            *acc_err = ideal_y - *y;
        }
    }
}
