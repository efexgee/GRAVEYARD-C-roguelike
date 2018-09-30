#include "los.h"
#include <stdio.h>

/*
void draw_line(level *lvl, int cur_x, int cur_y, int target_x, int target_y) {
    slope = slope(cur_x, cur_y, target_x, target_y)
    dy = slope / (target_x - cur_x)
    // one step?
    acc_error = 0
    until next_square = end
        next_square(&cur_x, &cur_y, dy, &acc_error)
        draw(lvl, cur_x, cur_y)
*/

void next_square(int *x, int *y, float dy, int *acc_err) {
    // Change a position to the next position along an angle (sort of)
    // This requires dy and acc_err to be passed in so this really can't
    // exist on its own. That feels weird.

    // sanity check
    fprintf(stderr, "%s(%d, %d, %.2f, %d)\n", "next_square", *x, *y, dy, *acc_err);

    if (dy < 0) {
        fprintf(stderr, "%s: dy is negative\n", "next_square");
        return;
    } else if (dy == 0) {
        fprintf(stderr, "%s: dy is zero\n", "next_square");
        return;
    } else if (dy == INFINITY) {
        fprintf(stderr, "%s: dy is infinity\n", "next_square");
        return;
    }

    int ideal_y, adjust;

    // Can't go plus 1 everywhere! Might need to go backwards!!!
    // *x++ <-- Nooooooooooooooo!
    (*x)++;
    ideal_y = *y + dy;

    *y = round(ideal_y);
    // y = round(ideal_y = *y + dy);  :.(
    
    *acc_err += *y - ideal_y;

    if (abs(*acc_err > 0.5)) {
        adjust = *acc_err < 0 ? 1 : -1;

        *y += adjust;
        *acc_err += adjust;
    }

    //assert(abs(*acc_err <= 0.5));
}
