#include <stdio.h>

#include "los.h"

#define X_STEP 1;
#define Y_STEP 0.27;
#define NUM_STEPS 8;


void main() {
    int num_steps = NUM_STEPS;
    int x = 0;
    int y = 0;
    int x_step = X_STEP;
    float y_step = Y_STEP;
    float acc_err = 0.0;

    printf("(x,y)=(%d, %d), dx=%d, dy=%.2f, err=%.2f\n", x, y, x_step, y_step, acc_err);

    for (int i=0; i < num_steps ; i++) {
        printf("\n");
        next_square(&x, &y, x_step, y_step, &acc_err);
        printf("(x,y)=(%d, %d), dx=%d, dy=%.2f, err=%.2f\n", x, y, x_step, y_step, acc_err);
    }
}
