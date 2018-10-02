#include "los.h"
#include <stdio.h>

/*
void draw_line(level *lvl, int cur_x, int cur_y, int target_x, int target_y) {
    slope = slope(cur_x, cur_y, target_x, target_y)
    y_step = slope / (target_x - cur_x)
    // one step?
    acc_error = 0
    until next_square = end
        next_square(&cur_x, &cur_y, y_step, &acc_error)
        draw(lvl, cur_x, cur_y)
*/

void next_square(int *x, int *y, int x_step, float y_step, int *acc_err) {
    // Change a position to the next position along an angle (sort of)
    // This requires y_step and acc_err to be passed in so this really can't
    // exist on its own. That feels weird.

    //fprintf(stderr, "%s((%d, %d), %d, %.2f, %d)\n", "next_square", *x, *y, x_step, y_step, *acc_err);

    int ideal_y, new_y, new_acc_err;

    // use abs() instead?
    if (y_step == INFINITY || y_step == -INFINITY) {
        // Just moving vertically.
        //fprintf(stderr, "%s: y_step is infinity\n", "next_square");
        *y += y_step < 0 ? -1 : 1;
        //fprintf(stderr, "set y = %d\n", *y);
        return;
    }

    // *x++ <-- Nooooooooooooooo!
    *x += x_step;
    //fprintf(stderr, "set x = %d\n", *x);

    if (y_step == 0) {
        // Just moving horizontally
        //fprintf(stderr, "%s: y_step is zero\n", "next_square");
        return;
    }

    // exact y
    ideal_y = *y + y_step;

    // proposed new y
    new_y = round(ideal_y);

    // new acc err with proposed new y
    new_acc_err = *acc_err + (new_y - ideal_y);

    // is new acc err too large?
    if (abs(new_acc_err) > 0.5) {
        new_y = round(ideal_y + (new_acc_err < 0 ? 0.5 : -0.5));
        new_acc_err = *acc_err + (new_y - ideal_y);
    }

    *y = new_y;
    *acc_err = new_acc_err;
}
