// #include <math.h>
// #include <assert.h>

#include "los.h"

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
    int ideal_y, adjust;

    *x++;
    ideal_y = *y + dy;

    *y = round(ideal_y);
    // y = round(ideal_y = *y + dy);  :.(
    
    *acc_err += *y - ideal_y;

    if (abs(*acc_err > 0.5)) {
        adjust = *acc_err < 0 ? 1 : -1;

        *y += adjust;
        *acc_err += adjust;
    }

    assert(abs(*acc_err <= 0.5));
}

// below here needs to be in game.c because checking validity
// of a square requires information outside of this lib

/*
bool approach(level *lvl, mobile *actor, int target_x, int target_y) {
    // This would be a function that "takes one step" and
    // only exists to provide the dy/acc_err wrapper around
    // next_square. That means duplicating code in a different
    // place. Is that better?

    // I guess this makes sense because this is a case where
    // we are not reusing dy or acc_err so we are not benefitting
    // from the efficiency of the algorithm or its smoothing. I.e.
    // this is no longer Bresenham

    int acc_err; // Doesn't actually get used
    int dy = (b_y - actor->y) / (b_x - actor->x);

    int new_x = actor->x;
    int new_y = actor->y;

    next_square(&new_x, &new_y, dy, &acc_err);

    // is_position_valid() is inside game.c
    if (is_position_valid(lvl, new_x, new_y)) {
        actor->x = new_x;
        actor->y = new_y;
        return true;
    } else {
        return false;
    }
}

bool can_see(mobile *actor, target_x, target_y) {
    // This is between a thing and a position
    // It just wraps line_of_sight for easier English reading
    // Making a thing-to-thing function seems too specific
    return (line_of_sight(actor->x, actor->y, target_x, target_y));
}

bool line_of_sight(level *lvl, int a_x, int a_y, int b_x, int b_y) {
    // This is between two positions, theoretically non-directional

    int acc_err = 0;
    int dy = (b_y - a_y) / (b_x - a_x);

    while (!(a_x == b_x && a_y == b_y)) {
        next_square(&a_x, &a_y, dy, &acc_err);
        // is_position_valid() is inside game.c
        if (!(is_position_valid(lvl, a_x, a_y))) {
            return false;
        }

    return true;
}
*/
