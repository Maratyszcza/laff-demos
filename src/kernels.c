#include <kernels.h>

void dgemm_naive(size_t size,
	const double a[restrict static size*size],
	const double b[restrict static size*size],
	double c[restrict static size*size])
{
	for (size_t i = 0; i < size; i++) {
		for (size_t j = 0; j < size; j++) {
			double sum0 = c[i*size+j], sum1 = 0.0, sum2 = 0.0, sum3 = 0.0;
			const double *restrict local_a = &a[i*size];
			const double *restrict local_b = &b[j];
			size_t k = size;
			for (; k >= 4; k -= 4) {
				sum0 += local_a[0] * (*local_b);
				local_b += size;
				sum1 += local_a[1] * (*local_b);
				local_b += size;
				sum2 += local_a[2] * (*local_b);
				local_b += size;
				sum3 += local_a[3] * (*local_b);
				local_b += size;
				local_a += 4;
			}
			if (k != 0) {
				sum0 += local_a[0] * local_b[0];
				if (k >= 2) {
					sum1 += local_a[1] * local_b[size];
					if (k >= 3) {
						sum2 += local_a[2] * local_b[2*size];
					}
				}
			}
			c[i*size+j] = (sum0 + sum1) + (sum2 + sum3);
		}
	}
}

static inline size_t minu(size_t a, size_t b) {
	return a < b ? a : b;
}

void dgemm_blocked(size_t size,
	const double a[restrict static size*size],
	const double b[restrict static size*size],
	double c[restrict static size*size])
{
	const size_t block_size = 26;
	for (size_t i = 0; i < size; i += block_size) {
		for (size_t j = 0; j < size; j += block_size) {
			for (size_t k = 0; k < size; k += block_size) {
				for (size_t ii = i; ii < minu(i + block_size, size); ii++) {
					for (size_t jj = j; jj < minu(j + block_size, size); jj++) {
						double sum0 = c[ii*size+jj], sum1 = 0.0, sum2 = 0.0, sum3 = 0.0;
						const double *restrict local_a = &a[ii*size+k];
						const double *restrict local_b = &b[k*size+jj];
						size_t kk = minu(block_size, size - k);
						for (; kk >= 4; kk -= 4) {
							sum0 += local_a[0] * (*local_b);
							local_b += size;
							sum1 += local_a[1] * (*local_b);
							local_b += size;
							sum2 += local_a[2] * (*local_b);
							local_b += size;
							sum3 += local_a[3] * (*local_b);
							local_b += size;
							local_a += 4;
						}
						if (k != 0) {
							sum0 += local_a[0] * local_b[0];
							if (k >= 2) {
								sum1 += local_a[1] * local_b[size];
								if (k >= 3) {
									sum2 += local_a[2] * local_b[2*size];
								}
							}
						}
						c[ii*size+jj] = (sum0 + sum1) + (sum2 + sum3);
					}					
				}
			}
		}
	}
}

void chase_pointers(void** data) {
	void* start = *data;
	void* current = start;
	do {
		current = *((void* volatile*) current);
	} while (current != start);
}
