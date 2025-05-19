#ifndef MATH_TEST_H
#define MATH_TEST_H

#include "Portfolio.h"
std::vector<double> mean_test(Portfolio &test, timestamp start);
Doubles_Matrix covariance_test(Portfolio &test, timestamp start);
Doubles_Matrix cholesky_test(Portfolio &test, timestamp start);
int QR_decomposition_test(Portfolio &test, timestamp start);
std::vector<std::pair<double, std::vector<double>>> eigen_test(Portfolio &test,
                                                               timestamp start);

#endif // MATH_TEST_H
