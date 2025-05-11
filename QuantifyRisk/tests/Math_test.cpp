#include "Math_test.h"
#include "math/numerical.h"

std::vector<double> mean_test(Portfolio &test, timestamp start) {
	// Test the mean function
	int info = test.calculate_aligned_metrics(start);
	if (info != 0) {
		std::cerr << "Error calculating metrics" << std::endl;
		return {};
	}
	return test.aligned_means;
}

Doubles_Matrix covariance_test(Portfolio &test, timestamp start) {
	// Test the covariance function
	Doubles_Matrix aligned = test.aligned_log_returns(start);
	Doubles_Matrix cov = test.calculate_covariance(start);
	return cov;
}

Doubles_Matrix cholesky_test(Portfolio &test, timestamp start) {
	// Test the Cholesky decomposition function
	Doubles_Matrix cov = test.calculate_covariance(start);
	Doubles_Matrix cholesky = math::cholesky_decomposition(cov);
	return cholesky;
}
