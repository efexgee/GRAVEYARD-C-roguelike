#ifndef HELPERS_H
#define HELPERS_H

//TODO rename this file - macros.h ?
//TODO put into dir structure somewhere

#define frand() ((float)rand() / RAND_MAX)

#define prob(p) (frand() <= p)

//TODO do we want this?
//TODO do we want 1..N?
#define choose(n) (rand() % (n + 1) + 1)

#endif
