#include <xorshift.h>
#include <assert.h>

struct xor_shift xor_shift_init(uint32_t state, uint32_t bits) {
	assert(bits <= 31);
	struct xor_shift xor_shift;
	xor_shift.bits = bits;
	xor_shift.mask = (UINT32_C(1) << bits) - UINT32_C(1);
	xor_shift.state = state & xor_shift.mask;
	switch (bits) {
		case 30:
			xor_shift.shift[0] = 1;
			xor_shift.shift[1] = 11;
			xor_shift.shift[2] = 24;
			break;
		case 29:
			xor_shift.shift[0] = 2;
			xor_shift.shift[1] = 4;
			xor_shift.shift[2] = 5;
			break;
		case 28:
			xor_shift.shift[0] = 1;
			xor_shift.shift[1] = 3;
			xor_shift.shift[2] = 9;
			break;
		case 27:
			xor_shift.shift[0] = 1;
			xor_shift.shift[1] = 2;
			xor_shift.shift[2] = 6;
			break;
		case 26:
			xor_shift.shift[0] = 2;
			xor_shift.shift[1] = 5;
			xor_shift.shift[2] = 3;
			break;
		case 25:
			xor_shift.shift[0] = 2;
			xor_shift.shift[1] = 1;
			xor_shift.shift[2] = 6;
			break;
		case 24:
			xor_shift.shift[0] = 6;
			xor_shift.shift[1] = 1;
			xor_shift.shift[2] = 9;
			break;
		case 23:
			xor_shift.shift[0] = 7;
			xor_shift.shift[1] = 8;
			xor_shift.shift[2] = 2;
			break;
		case 22:
			xor_shift.shift[0] = 5;
			xor_shift.shift[1] = 3;
			xor_shift.shift[2] = 2;
			break;
		case 21:
			xor_shift.shift[0] = 4;
			xor_shift.shift[1] = 3;
			xor_shift.shift[2] = 6;
			break;
		case 20:
			xor_shift.shift[0] = 2;
			xor_shift.shift[1] = 1;
			xor_shift.shift[2] = 9;
			break;
		case 19:
			xor_shift.shift[0] = 9;
			xor_shift.shift[1] = 6;
			xor_shift.shift[2] = 4;
			break;
		case 18:
			xor_shift.shift[0] = 3;
			xor_shift.shift[1] = 9;
			xor_shift.shift[2] = 8;
			break;
		case 17:
			xor_shift.shift[0] = 9;
			xor_shift.shift[1] = 6;
			xor_shift.shift[2] = 1;
			break;
		case 16:
			xor_shift.shift[0] = 8;
			xor_shift.shift[1] = 9;
			xor_shift.shift[2] = 5;
			break;
		case 15:
			xor_shift.shift[0] = 4;
			xor_shift.shift[1] = 1;
			xor_shift.shift[2] = 9;
			break;
		case 14:
			xor_shift.shift[0] = 9;
			xor_shift.shift[1] = 7;
			xor_shift.shift[2] = 2;
			break;
		case 13:
			xor_shift.shift[0] = 8;
			xor_shift.shift[1] = 5;
			xor_shift.shift[2] = 2;
			break;
		case 12:
			xor_shift.shift[0] = 7;
			xor_shift.shift[1] = 5;
			xor_shift.shift[2] = 9;
			break;
		case 11:
			xor_shift.shift[0] = 6;
			xor_shift.shift[1] = 4;
			xor_shift.shift[2] = 7;
			break;
		case 10:
			xor_shift.shift[0] = 5;
			xor_shift.shift[1] = 1;
			xor_shift.shift[2] = 3;
			break;
		case 9:
			xor_shift.shift[0] = 8;
			xor_shift.shift[1] = 4;
			xor_shift.shift[2] = 5;
			break;
		case 8:
			xor_shift.shift[0] = 3;
			xor_shift.shift[1] = 1;
			xor_shift.shift[2] = 5;
			break;
		case 7:
			xor_shift.shift[0] = 5;
			xor_shift.shift[1] = 2;
			xor_shift.shift[2] = 6;
			break;
		case 6:
			xor_shift.shift[0] = 2;
			xor_shift.shift[1] = 3;
			xor_shift.shift[2] = 1;
			break;
		case 5:
			xor_shift.shift[0] = 4;
			xor_shift.shift[1] = 3;
			xor_shift.shift[2] = 1;
			break;
		case 4:
			xor_shift.shift[0] = 3;
			xor_shift.shift[1] = 1;
			xor_shift.shift[2] = 2;
			break;
		default:
			__builtin_unreachable();
	}
	return xor_shift;
}
