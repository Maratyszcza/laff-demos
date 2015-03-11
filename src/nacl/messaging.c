#include <stddef.h>
#include <string.h>

#include <ppapi/c/pp_var.h>

#include <nacl/interfaces.h>
#include <nacl/stringvars.h>
#include <benchmark.h>

static void handleMessage(PP_Instance instance, struct PP_Var messageVar) {
	struct PP_Var benchmarkVar = PP_MakeUndefined();
	struct PP_Var sizeVar = PP_MakeUndefined();

	if (messageVar.type != PP_VARTYPE_DICTIONARY) {
		consoleInterface->Log(instance, PP_LOGLEVEL_ERROR, stringVarErrorMessageType);
		__builtin_unreachable();
	}

	benchmarkVar = dictionaryInterface->Get(messageVar, stringVarBenchmark);
	if (benchmarkVar.type == PP_VARTYPE_UNDEFINED) {
		consoleInterface->Log(instance, PP_LOGLEVEL_ERROR, stringVarErrorBenchmarkUnspecified);
		__builtin_unreachable();
	}
	if (benchmarkVar.type != PP_VARTYPE_STRING) {
		consoleInterface->Log(instance, PP_LOGLEVEL_ERROR, stringVarErrorBenchmarkType);
		__builtin_unreachable();
	}
	uint32_t benchmarkLength = 0;
	const char* benchmarkPointer = varInterface->VarToUtf8(benchmarkVar, &benchmarkLength);
	if (benchmarkPointer == NULL) {
		consoleInterface->Log(instance, PP_LOGLEVEL_ERROR, stringVarErrorBenchmarkType);
		__builtin_unreachable();
	}
	enum BenchmarkType benchmarkType;
	if (strncmp(benchmarkPointer, "BLIS", benchmarkLength) == 0) {
		benchmarkType = BenchmarkTypeBlisDGEMM;
	} else if (strncmp(benchmarkPointer, "Naive", benchmarkLength) == 0) {
		benchmarkType = BenchmarkTypeNaiveDGEMM;
	} else if (strncmp(benchmarkPointer, "Blocked", benchmarkLength) == 0) {
		benchmarkType = BenchmarkTypeBlockedDGEMM;
	} else if (strncmp(benchmarkPointer, "Latency", benchmarkLength) == 0) {
		benchmarkType = BenchmarkTypePointerChasing;
	} else {
		consoleInterface->Log(instance, PP_LOGLEVEL_ERROR, stringVarErrorBenchmarkValue);
		__builtin_unreachable();
	}

	sizeVar = dictionaryInterface->Get(messageVar, stringVarSize);
	if (sizeVar.type == PP_VARTYPE_UNDEFINED) {
		consoleInterface->Log(instance, PP_LOGLEVEL_ERROR, stringVarErrorSizeUnspecified);
		__builtin_unreachable();
	}
	if (sizeVar.type != PP_VARTYPE_INT32) {
		consoleInterface->Log(instance, PP_LOGLEVEL_ERROR, stringVarErrorSizeType);
		__builtin_unreachable();
	}
	const int32_t size = sizeVar.value.as_int;
	if (size <= 0) {
		consoleInterface->Log(instance, PP_LOGLEVEL_ERROR, stringVarErrorSizeValue);
		__builtin_unreachable();
	}
	if (benchmarkType == BenchmarkTypePointerChasing) {
		if ((size & (size - 1)) != 0) {
			consoleInterface->Log(instance, PP_LOGLEVEL_ERROR, stringVarErrorSizeNP2);
			__builtin_unreachable();
		}
	}

	struct BenchmarkResult benchmarkResult = {
		.time = __builtin_nan(""),
		.flops = __builtin_nan(""),
		.throughput = __builtin_nan("")
	};
	runBenchmark(benchmarkType, size, &benchmarkResult);

	dictionaryInterface->Set(messageVar, stringVarFLOPS, PP_MakeDouble(benchmarkResult.flops));
	dictionaryInterface->Set(messageVar, stringVarTime, PP_MakeDouble(benchmarkResult.time));

	messagingInterface->PostMessage(instance, messageVar);

cleanup:
	varInterface->Release(messageVar);
	varInterface->Release(benchmarkVar);
	varInterface->Release(sizeVar);
}

const struct PPP_Messaging_1_0 pluginMessagingInterface = {
	.HandleMessage = handleMessage
};
