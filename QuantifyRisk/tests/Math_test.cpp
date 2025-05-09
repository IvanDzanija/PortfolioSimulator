#include "Math_test.h"

std::vector<double> mean_test(void) {
	// Test the mean function
	Portfolio test = alignment_test();
	Doubles_Matrix aligned = test.aligned_log_returns();
	std::vector<double> means = test.calculate_aligned_means();
	std::cout << "Means:" << std::endl;
	for (const auto &value : means) {
		std::cout << value << ' ';
	}
	std::cout << std::endl;
	return means;
}

Doubles_Matrix covariance_test(void) {
	// Test the covariance function
	Portfolio test = alignment_test();
	Doubles_Matrix aligned = test.aligned_log_returns();
	Doubles_Matrix cov = test.calculate_covariance();
	std::cout << "Covariance:" << std::endl;
	for (const auto &row : cov) {
		for (const auto &value : row) {
			std::cout << value << ' ';
		}
		std::cout << std::endl;
	}
	return cov;
}
