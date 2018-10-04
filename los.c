#include "los.h"
#include <stdio.h>

void next_square(int *x, int *y, int x_step, float y_step, float *acc_err) {
    // Change a position to the next position along an angle (sort of)
    // This requires y_step and acc_err to be passed in so this really can't
    // exist on its own. That feels weird.

    //fprintf(stderr, "%s((%d, %d), %d, %.2f, %d)\n", "next_square", *x, *y, x_step, y_step, *acc_err);

    float ideal_y, new_acc_err;
    int new_y;

    // use abs() instead?
    if (y_step == INFINITY || y_step == -INFINITY) {
        // Just moving vertically.
        //fprintf(stderr, "%s: y_step is infinity\n", "next_square");
        *y += y_step < 0 ? -1 : 1;
        return;
    }

    *x += x_step;

    if (y_step == 0) {
        // Just moving horizontally
        //fprintf(stderr, "%s: y_step is zero\n", "next_square");
        return;
    }

    // exact y
    ideal_y = *y + y_step;

    //printf("ideal_y=%f\n", ideal_y);

    // proposed new y
    new_y = round(ideal_y);

    //printf("new_y=%d\n", new_y);

    // new acc err with proposed new y
    new_acc_err = *acc_err + (new_y - ideal_y);

    //printf("new_y=%d new_acc_err=%f\n", new_y, new_acc_err);

    // is new acc err too large?
    if (fabs(new_acc_err) > 0.5) {
        //printf("adjusting...\n");
        new_y = round(ideal_y + (new_acc_err < 0 ? 0.5 : -0.5));
        new_acc_err += new_y - ideal_y;
    }

    //printf("new_y=%d new_acc_err=%f\n", new_y, new_acc_err);

    *y = new_y;
    *acc_err = new_acc_err;

    //printf("y=%d acc_err=%f\n", *y, *acc_err);
}
