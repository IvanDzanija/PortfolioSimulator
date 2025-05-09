#ifndef MATH_H
#define MATH_H
#include <cmath>
#include <stdexcept>
#include <vector>

namespace math {

template <typename T> double mean(const std::vector<T> &data) {
	// O(n)
	// Calculate the mean of a vector
	double sum = 0.0;
	for (const T &value : data) {
		sum += value;
	}
	return sum / data.size();
}

template <typename T>
double covariance(const std::vector<T> &x, T mean_x, const std::vector<T> &y,
				  T mean_y) {
	size_t n = x.size();
	//	double mean_x = mean(x);
	//	double mean_y = mean(y);
	double cov = 0.0;

	for (size_t i = 0; i < n; ++i) {
		cov += (x.at(i) - mean_x) * (y.at(i) - mean_y);
	}
	return cov / (n - 1);
}

template <typename T>
std::vector<std::vector<T>>
matrix_transpose(const std::vector<std::vector<T>> &matrix) {
	// O(n^2)
	// Transpose a matrix
	size_t rows = matrix.size();
	size_t cols = matrix[0].size();

	std::vector<std::vector<T>> transposed(cols, std::vector<T>(rows, 0));

	for (size_t i = 0; i < rows; ++i) {
		for (size_t j = 0; j < cols; ++j) {
			transposed[j][i] = matrix[i][j];
		}
	}
	return transposed;
}

template <typename T>
std::vector<std::vector<T>>
cholesky_decomposition(const std::vector<std::vector<T>> &matrix) {
	// O(n^3 / 3)
	// Works only for symmetric positive-definite matrices
	size_t n = matrix.size();
	if (n == 0 || matrix[0].size() != n) {
		throw std::invalid_argument("Matrix must be square and non-empty!");
	}

	std::vector<std::vector<T>> L(n, std::vector<T>(n, 0));

	for (size_t i = 0; i < n; ++i) {
		for (size_t j = 0; j <= i; ++j) {
			T sum = 0;
			for (size_t k = 0; k < j; ++k) {
				sum += L[i][k] * L[j][k];
			}
			if (i == j) {
				if (matrix[i][i] - sum <= 0) {
					throw std::runtime_error(
						"Matrix is not positive definite!");
				}
				L[i][j] = std::sqrt(matrix[i][i] - sum);
			} else {
				L[i][j] = (matrix[i][j] - sum) / L[j][j];
			}
		}
	}
	return L;
}

} // namespace math

#endif // MATH_H
