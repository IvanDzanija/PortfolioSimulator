#include <cmath>
#include <stdexcept>
#include <vector>

template <typename T>
std::vector<std::vector<T>>
cholesky_decomposition(const std::vector<std::vector<T>> &matrix) {
	int n = matrix.size();
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
