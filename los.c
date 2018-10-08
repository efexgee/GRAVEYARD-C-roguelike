#include "los.h"
#include <stdio.h>

#define TRUE 1
#define FALSE 0

void update(int *stepper, int *bumper, float slope, int step, int bump, float *err) {
    // calculate where the next point should be
    // make sure this ends up as a float!
    float ideal = *bumper + *err + slope;

    fprintf(stderr, "[in](s%2d, b%2d) m %3.2f s %2d b %2d ideal %6.2f err", *stepper, *bumper, slope, step, bump, ideal, *err);

    // increment the stepper no matter what
    *stepper += step;

    int bumped = FALSE;

    if (fabs(ideal - *bumper) >= 0.5) {
        // more than half-way, round up
        *bumper += bump;
        bumped = TRUE;
        // new error is distance down from new bumper to the ideal
        *err = ideal - *bumper;
    } else {
        // less than half-way, leave bumper as is
        // new error is increased by distance from bumper to ideal
        *err += slope;
    }

    //fprintf(stderr, "\n");
    //fprintf(stderr, "[out %s] (s%2d, b%2d) m %3.2f s %2d b %2d ideal %6.2f err %6.2f %5.2f\n", "next_square", *stepper, *bumper, slope, step, bump, ideal, *err, bumped ? 1 : ideal - *bumper);
    fprintf(stderr, "[out](s%2d, b%2d) err %6.2f %5.2f\n", *stepper, *bumper, *err, bumped ? 1 : ideal - *bumper);
}

void next_square(int *x, int *y, int x_direction, float slope, float *err) {
    // Change a position to the next position along an angle (sort of)
    // This requires slope and err to be passed in so this really can't
    // exist on its own. That feels weird.

    //fprintf(stderr, "%s((%d, %d), x_dir=%d, slope=%.2f, err=%.2f)\n", "next_square", *x, *y, x_direction, slope, *err);

    // Handle vertical lines
    if (slope == INFINITY) {
        *y += x_direction;
        return;
    } else if (slope == -INFINITY) {
        *y -= x_direction;
        return;
    }

    // Handle horizontal lines
    if (slope == 0) {
        *x += x_direction;
        return;
    }

    // Set defaults to save typing in the if forest
    int *stepper = x;
    int *bumper = y;
    int step = 1;
    int bump = 1;

    if (x_direction >= 0) {
    // Increasing X
        if (slope < 0) {
        // Negative slope
            if (fabs(slope) <= 1) {
                // Octant I
                fprintf(stderr, "Octant %-4s ", "I");
                bump = -1;
            } else {
                // Octant II
                fprintf(stderr, "Octant %-4s ", "II");
                stepper = y;
                step = -1;
                bumper = x;
                slope *= -1;
                slope = 1 / slope;
            }
        } else {
        // Positive slope
            if (fabs(slope) > 1) {
                // Octant VII
                fprintf(stderr, "Octant %-4s ", "VII");
                stepper = y;
                bumper = x;
                slope = 1 / slope;
            } else {
                // Octant VIII
                fprintf(stderr, "Octant %-4s ", "VIII");
                // Yay! Lazytown!
            }
        }
    } else {
    // Decreasing X
        if (slope >= 0) {
        // Positive slope
            if (fabs(slope) > 1) {
                // Octant III
                fprintf(stderr, "Octant %-4s ", "III");
                stepper = y;
                bumper = x;
                step = -1;
                bump = -1;
                slope *= -1;
                slope = 1 / slope;
            } else {
                // Octant IV
                fprintf(stderr, "Octant %-4s ", "IV");
                step = -1;
                bump = -1;
                slope *= -1;
            }
        } else {
        // Negative slope
            if (fabs(slope) <= 1) {
                // Octant V
                fprintf(stderr, "Octant %-4s ", "V");
                step = -1;
                slope *= -1;
            } else {
                // Octant VI
                fprintf(stderr, "Octant %-4s ", "VI");
                stepper = y;
                bumper = x;
                bump = -1;
                slope = 1 / slope;
            }
        }
    }

    fprintf(stderr, "x %c ", (x_direction < 0 ? '<' : '>'));
    //fprintf(stderr, "\n");
    update(stepper, bumper, slope, step, bump, err);
}
