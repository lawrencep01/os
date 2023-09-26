#ifndef LCG_PARKMILLER_H
#define LCG_PARKMILLER_H

#define RANDOM_MAX ((1u << 31u) - 1u)

unsigned lcg_parkmiller(unsigned *state);
unsigned next_random();

#endif
