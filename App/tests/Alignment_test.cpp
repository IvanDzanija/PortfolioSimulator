#include "Alignment_test.h"

Doubles_Matrix alignment_test(Portfolio &test, timestamp start) {
    Doubles_Matrix aligned = test.aligned_log_returns(start);

    return aligned;
}

Doubles_Matrix remainder_test(Portfolio &test, timestamp start) {
    Doubles_Matrix remainder = test.future_log_returns(start);

    return remainder;
}
