#pragma once

enum BenchmarkType {
	BenchmarkTypeNaiveDGEMM     = 1,
	BenchmarkTypeBlockedDGEMM   = 2,
	BenchmarkTypeBlisDGEMM      = 3,
	BenchmarkTypePointerChasing = 4,
};

struct BenchmarkResult {
	double time;
	double flops;
	double throughput;
};

extern void runBenchmark(enum BenchmarkType benchmarkType, int32_t size, struct BenchmarkResult* out);
