#include <ppapi/c/pp_var.h>

extern struct PP_Var stringVarErrorMessageType;
extern struct PP_Var stringVarErrorBenchmarkUnspecified;
extern struct PP_Var stringVarErrorBenchmarkType;
extern struct PP_Var stringVarErrorBenchmarkValue;
extern struct PP_Var stringVarErrorDatatypeUnspecified;
extern struct PP_Var stringVarErrorDatatypeType;
extern struct PP_Var stringVarErrorDatatypeValue;
extern struct PP_Var stringVarErrorSizeUnspecified;
extern struct PP_Var stringVarErrorSizeType;
extern struct PP_Var stringVarErrorSizeValue;
extern struct PP_Var stringVarErrorSizeNP2;

extern struct PP_Var stringVarBenchmark;
extern struct PP_Var stringVarDatatype;
extern struct PP_Var stringVarSize;
extern struct PP_Var stringVarTime;
extern struct PP_Var stringVarFLOPS;
extern struct PP_Var stringVarBandwidth;

extern struct PP_Var stringVarSingle;
extern struct PP_Var stringVarDouble;
extern struct PP_Var stringVarComplexSingle;
extern struct PP_Var stringVarComplexDouble;

extern void stringVarsInit();
extern void stringVarsRelease();
