#include "Alignment_test.h"

Doubles_Matrix alignment_test(Portfolio &test, timestamp start) {
	Doubles_Matrix aligned = test.aligned_log_returns(start);

	return aligned;
}
