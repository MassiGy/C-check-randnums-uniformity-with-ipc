//  @https://en.wikipedia.org/wiki/Multiply-with-carry_pseudorandom_number_generator
// C99 Complementary Multiply With Carry generator
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// How many bits in rand()?
// https://stackoverflow.com/a/27593398
#define LOG_1(n) (((n) >= 2) ? 1 : 0)
#define LOG_2(n) (((n) >= 1<<2) ? (2 + LOG_1((n)>>2)) : LOG_1(n))
#define LOG_4(n) (((n) >= 1<<4) ? (4 + LOG_2((n)>>4)) : LOG_2(n))
#define LOG_8(n) (((n) >= 1<<8) ? (8 + LOG_4((n)>>8)) : LOG_4(n))
#define LOG(n)   (((n) >= 1<<16) ? (16 + LOG_8((n)>>16)) : LOG_8(n))
#define BITS_TO_REPRESENT(n) (LOG(n) + !!((n) & ((n) - 1)))
#if ((RAND_MAX | (RAND_MAX >> 1)) != RAND_MAX) 
#error "expected a RAND_MAX that is 2^n - 1!"
#endif
#define RAND_BITS BITS_TO_REPRESENT(RAND_MAX)

// CMWC working parts
#define CMWC_CYCLE 4096         // as Marsaglia recommends
#define CMWC_C_MAX 809430660    // as Marsaglia recommends

struct cmwc_state {
	uint32_t Q[CMWC_CYCLE];
	uint32_t c;	// must be limited with CMWC_C_MAX
	unsigned i;
};

// Collect 32 bits of rand(). You are encouraged to use a better source instead.
uint32_t rand32(void);

// Init the state with seed
void initCMWC(struct cmwc_state *state, unsigned int seed);

// CMWC engine
uint32_t randCMWC(struct cmwc_state *state);  //EDITED parameter *state was missing






