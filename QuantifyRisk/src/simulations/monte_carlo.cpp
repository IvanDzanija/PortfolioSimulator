#include "Portfolio.h"

Doubles_Matrix Portfolio::calculate_covariance() {
	if (covariance_matrix_calculated) {
		return covariance_matrix;
	}

	Doubles_Matrix returns = aligned_log_returns();
	int rows = returns.size();
	int cols = returns[0].size();
	Doubles_Matrix covariance(rows, std::vector<double>(rows, 0.0));

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < rows; ++j) {
			double mean_i = 0.0;
			double mean_j = 0.0;
			for (int k = 0; k < cols; ++k) {
				mean_i += returns[i][k];
				mean_j += returns[j][k];
			}
			mean_i /= cols;
			mean_j /= cols;

			for (int k = 0; k < cols; ++k) {
				covariance[i][j] +=
					(returns[i][k] - mean_i) * (returns[j][k] - mean_j);
			}
			covariance[i][j] /= (cols - 1);
		}
	}

	return covariance;
}
