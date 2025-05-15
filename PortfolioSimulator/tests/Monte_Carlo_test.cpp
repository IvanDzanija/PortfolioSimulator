#include "Monte_Carlo_test.h"

std::vector<Doubles_Matrix> monte_carlo_test(Portfolio &portfolio,
											 int simulations, int steps,
											 timestamp start) {
	return portfolio.monte_carlo(simulations, steps, start);
}
