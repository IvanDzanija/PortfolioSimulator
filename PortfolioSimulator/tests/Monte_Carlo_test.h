#ifndef MONTE_CARLO_TEST_H
#define MONTE_CARLO_TEST_H
#include "Portfolio.h"

std::vector<Doubles_Matrix> monte_carlo_test(Portfolio &portfolio,
											 int simulations, int steps,
											 timestamp start);
#endif // MONTE_CARLO_TEST_H
