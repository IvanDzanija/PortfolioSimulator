#include "Portfolio.h"

std::vector<double> Portfolio::calculate_aligned_means() {
	Doubles_Matrix returns = aligned_log_returns();
	int rows = returns.size();	  // number of assets
	int cols = returns[0].size(); // number of candles

	std::vector<double> means(rows);
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			means.at(i) += returns.at(i).at(j);
		}
		means.at(i) /= cols;
	}

	return means;
}

Doubles_Matrix Portfolio::calculate_covariance() {
	if (covariance_matrix_calculated) {
		return covariance_matrix;
	}

	Doubles_Matrix returns = aligned_log_returns();
	std::vector<double> means = calculate_aligned_means();
	int rows = returns.size();		 // number of assets
	int cols = returns.at(0).size(); // number of candles
	std::cout << "rows: " << rows << ", cols: " << cols << std::endl;
	Doubles_Matrix covariance(rows, std::vector<double>(rows));

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < rows; ++j) {
			if (i == j) {
				covariance.at(i).at(j) = 1.0;
			} else {
				covariance.at(i).at(j) = math::covariance(
					returns.at(i), means.at(i), returns.at(j), means.at(j));
			}
		}
	}

	return covariance;
}
