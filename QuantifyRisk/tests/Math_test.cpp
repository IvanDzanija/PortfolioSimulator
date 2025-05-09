#include "Math_test.h"

std::vector<double> mean_test(Portfolio &test) {
	// Test the mean function
	std::vector<double> means = test.calculate_aligned_means();
	return means;
}

Doubles_Matrix covariance_test(Portfolio &test) {
	// Test the covariance function
	Doubles_Matrix aligned = test.aligned_log_returns();
	Doubles_Matrix cov = test.calculate_covariance();
	return cov;
}
