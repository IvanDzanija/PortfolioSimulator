#include "Alignment_test.h"

Doubles_Matrix alignment_test(Portfolio &test) {
	Doubles_Matrix aligned = test.aligned_log_returns();

	return aligned;
}
