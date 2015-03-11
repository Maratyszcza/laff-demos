#include <stdint.h>

struct xor_shift {
	uint32_t state;
	uint32_t shift[3];
	uint32_t mask;
	uint32_t bits;
};

extern struct xor_shift xor_shift_init(uint32_t state, uint32_t bits);

inline static uint32_t xor_shift_next(struct xor_shift xor_shift[restrict static 1]) {
	uint32_t state = xor_shift->state;
	state ^= state >> xor_shift->shift[0];
	state ^= state << xor_shift->shift[1];
	state &= xor_shift->mask;
	state ^= state >> xor_shift->shift[2];
	xor_shift->state = state;
	return state;
}
