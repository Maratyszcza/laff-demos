#include <stddef.h>
#include <math.h>
#include <xorshift.h>

#if defined(__native_client__)
	#include <nacl/interfaces.h>
	static double time_sec() {
		return coreInterface->GetTimeTicks();
	}
#elif defined(EMSCRIPTEN)
	#include <emscripten.h>
	static double time_sec() {
		return emscripten_get_now() * 1.0e-3;
	}
#else
	#error "Platform-specific hack required"
#endif

#include <blis/blis.h>

#include <benchmark.h>
#include <kernels.h>

void runBenchmark(enum BenchmarkType benchmarkType, int32_t size, struct BenchmarkResult* out) {
	const int32_t experiments = 10;
	struct BenchmarkResult result = {
		.time = __builtin_nan(""),
		.flops = __builtin_nan(""),
		.throughput = __builtin_nan("")
	};

	switch (benchmarkType) {
		case BenchmarkTypeNaiveDGEMM:
		{
			double* a = malloc(size * size * sizeof(double));
			double* b = malloc(size * size * sizeof(double));
			double* c = malloc(size * size * sizeof(double));

			for (int32_t i = 0; i < size * size; i++) {
				a[i] = ((double) rand()) / ((double) RAND_MAX);
				b[i] = ((double) rand()) / ((double) RAND_MAX);
			}
			memset(c, 0, size * size * sizeof(double));

			for (int32_t experiment = 0; experiment < experiments; ++experiment) {
				const double timeStart = time_sec();
				dgemm_naive(size, a, b, c);
				result.time = fmin(result.time, time_sec() - timeStart);
			}
			result.flops = 2.0 * size * size * size / result.time;

			free(a);
			free(b);
			free(c);
			break;
		}
		case BenchmarkTypeBlockedDGEMM:
		{
			double* a = malloc(size * size * sizeof(double));
			double* b = malloc(size * size * sizeof(double));
			double* c = malloc(size * size * sizeof(double));

			for (int32_t i = 0; i < size * size; i++) {
				a[i] = ((double) rand()) / ((double) RAND_MAX);
				b[i] = ((double) rand()) / ((double) RAND_MAX);
			}
			memset(c, 0, size * size * sizeof(double));

			for (int32_t experiment = 0; experiment < experiments; ++experiment) {
				const double timeStart = time_sec();
				dgemm_blocked(size, a, b, c);
				result.time = fmin(result.time, time_sec() - timeStart);
			}
			result.flops = 2.0 * size * size * size / result.time;

			free(a);
			free(b);
			free(c);
			break;
		}
		case BenchmarkTypeBlisDGEMM:
		{
			obj_t alpha, beta;
			bli_obj_scalar_init_detached(BLIS_DOUBLE, &alpha);
			bli_obj_scalar_init_detached(BLIS_DOUBLE, &beta);
			bli_setsc( 1.0, 0.0, &alpha);
			bli_setsc( 0.0, 0.0, &beta);

			obj_t a, b, c;
			bli_obj_create(BLIS_DOUBLE, size, size, 0, 0, &a);
			bli_obj_create(BLIS_DOUBLE, size, size, 0, 0, &b);
			bli_obj_create(BLIS_DOUBLE, size, size, 0, 0, &c);
			bli_randm(&a);
			bli_randm(&b);
			bli_randm(&c);

			for (int32_t i = 0; i < experiments; ++i) {
				const double timeStart = time_sec();
				bli_gemm(&alpha, &a, &b, &beta, &c);
				result.time = fmin(result.time, time_sec() - timeStart);
			}
			result.flops = 2.0 * size * size * size / result.time;

			bli_obj_free(&a);
			bli_obj_free(&b);
			bli_obj_free(&c);
			break;
		}
		case BenchmarkTypePointerChasing:
		{
			struct xor_shift xor_shift = xor_shift_init(UINT32_C(1), __builtin_ctz(size));
			uint32_t last_index = 1;
			void** data = (void**) malloc(size * sizeof(void*));
			data[0] = &data[1];
			for (size_t i = 0; i < size; i++) {
				const uint32_t index = xor_shift_next(&xor_shift);
				data[last_index] = &data[index];
				last_index = index;
			}
			const int32_t iterations = 16777216 / size;
			for (int32_t experiment = 0; experiment < experiments; ++experiment) {
				const double timeStart = time_sec();
				for (int32_t iteration = 0; iteration < iterations; iteration++) {
					chase_pointers(data);
				}
				result.time = fmin(result.time, (time_sec() - timeStart) / ((double) iterations) / ((double) size));
			}
			free((void*) data);
			break;
		}
		default:
			__builtin_unreachable();
	}
	*out = result;
}
