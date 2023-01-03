#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

gmp_randstate_t state;

//Init the state and seeding the random algorithms
void randstate_init(uint64_t seed)
{
	srandom(seed);
	gmp_randinit_mt(state);
	gmp_randseed_ui(state, seed);
}

//Clearing the state
void randstate_clear(void)
{
	gmp_randclear(state);
}

