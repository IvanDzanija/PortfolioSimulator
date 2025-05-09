#include "Cryptocurrency.h"
#include "Portfolio.h"
#include "math/numerical.h"
#include "math/stohastic.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <random>

std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
std::vector<Doubles_Matrix> &Portfolio::monte_carlo(int simulations, int steps,
													timestamp start) {

	Doubles_Matrix changesurns = aligned_log_returns(start);
	Doubles_Matrix covariance = calculate_covariance(start);
	size_t n = changesurns.size(); // number of assets

	// Gather the starting prices for all assets
	std::vector<double> starting_prices(n);
	size_t index = 0;
	for (const auto &iter : assets) {
		const Cryptocurrency &crypto = iter.first;
		auto it =
			std::lower_bound(crypto.hist_data.begin(), crypto.hist_data.end(),
							 start, [](const Candle &candle, timestamp stamp) {
								 return candle.timestamp < stamp;
							 });
		starting_prices.at(index) = it->close;
	}

	std::vector<double> Z(n);
	for (int i = 0; i < n; ++i) {
		Z.at(i) = std::normal_distribution<double>(0.0, 1.0)(rnd);
	}

	Doubles_Matrix L = math::cholesky_decomposition(covariance);

	// Generate correlated shocks
	std::vector<double> Y = math::correlated_shocks(L, Z);

	std::vector<Doubles_Matrix> changes(
		simulations, Doubles_Matrix(n, std::vector<double>(steps)));
	std::vector<Doubles_Matrix> prices(
		simulations, Doubles_Matrix(n, std::vector<double>(steps)));

	double dt = 1.0;
	for (size_t i = 0; i < simulations; ++i) {
		for (size_t j = 0; j < n; ++j) {
			changes.at(i).at(j).at(0) = starting_prices.at(j);

			double mu = aligned_means.at(j);
			double sigma = aligned_volatilities.at(j);

			prices.at(i).at(j).at(0) = starting_prices.at(j);

			for (size_t k = 1; k < steps; ++k) {
				changes.at(i).at(j).at(k) =
					changes.at(i).at(j).at(k - 1) *
					std::exp((mu - 0.5 * sigma * sigma) * dt +
							 sigma * Y.at(j) * std::sqrt(dt));
				prices.at(i).at(j).at(k) = prices.at(i).at(j).at(k - 1) *
										   std::exp(changes.at(i).at(j).at(k));
			}
		}
	}
	return prices;
}
