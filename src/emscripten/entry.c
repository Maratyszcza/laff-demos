#include <emscripten.h>
#include <blis/blis.h>

int main(int argc, char** argv) {
	bli_init();
	EM_ASM(
		var benchmarkMap = {
			'Naive': 1,
			'Blocked': 2,
			'BLIS': 3,
			'Latency': 4
		};
		var onMsg = function(e) {
			var message = e.data;

			var benchmarkId = benchmarkMap[message.benchmark];
			var size = message.size|0;
			var resultPtr = Module._malloc(24);

			Module._runBenchmark(benchmarkId, size, resultPtr);

			message.time = Module.getValue(resultPtr, 'double');
			message.FLOPS = Module.getValue(resultPtr + 8, 'double');
			Module._free(resultPtr);

			postMessage(message);
		};
		addEventListener('message', onMsg, false);
		postMessage(null);
	);
	return 0;
}
