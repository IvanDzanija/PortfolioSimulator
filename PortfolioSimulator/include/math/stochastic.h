#ifndef STOCHASTIC_H
#define STOCHASTIC_H

#include <cmath>
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
std::vector<double> correlated_shocks(const std::vector<std::vector<T>> &L,
									  const std::vector<T> &Z) {
	size_t n = Z.size();
	std::vector<double> Y(n, 0.0);
	for (int i = 0; i < n; ++i)
		for (int k = 0; k <= i; ++k)
			Y[i] += L[i][k] * Z[k];
	return Y;
}
} // namespace math

#endif // STOCHASTIC_H
