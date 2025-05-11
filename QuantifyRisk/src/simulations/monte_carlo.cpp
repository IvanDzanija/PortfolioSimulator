#include "Cryptocurrency.h"
#include "Portfolio.h"
#include "math/numerical.h"
#include "math/stohastic.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <random>

std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
std::vector<Doubles_Matrix> Portfolio::monte_carlo(int simulations, int steps,
												   timestamp start) {

	Doubles_Matrix covariance = calculate_covariance(start);
	size_t n = covariance.size();				// number of assets
	size_t timeframe = covariance.at(0).size(); // number of candles

	// Gather the starting prices for all assets
	std::vector<double> starting_prices(n);
	size_t index = 0;
	for (const auto &iter : assets) {
		const Cryptocurrency &crypto = iter.first;

		// we check till aligned_end because start can be in the future and
		// gives pointer past-the-end
		auto it = std::lower_bound(crypto.hist_data.begin(),
								   crypto.hist_data.end(), aligned_end,
								   [](const Candle &candle, timestamp stamp) {
									   return candle.timestamp < stamp;
								   });
		starting_prices.at(index) = it->close;
		++index;
	}

	//	for (size_t i = 0; i < n; ++i) {
	//		std::cout << starting_prices.at(i) << std::endl;
	//	}

	std::vector<double> Z(n);
	for (size_t i = 0; i < n; ++i) {
		Z.at(i) = std::normal_distribution<double>(0.0, 1.0)(rnd);
		std::cout << Z.at(i) << std::endl;
	}

	Doubles_Matrix L = math::cholesky_decomposition(covariance);

	// Generate correlated shocks
	std::vector<double> Y = math::correlated_shocks(L, Z);

	std::vector<Doubles_Matrix> prices(
		simulations, Doubles_Matrix(n, std::vector<double>(steps)));

	double dt = 1.0;
	for (size_t i = 0; i < simulations; ++i) {
		for (size_t j = 0; j < n; ++j) {

			double mu = aligned_means.at(j);
			double sigma = aligned_volatilities.at(j);

			prices.at(i).at(j).at(0) = starting_prices.at(j);

			for (size_t k = 1; k < steps; ++k) {
				double change = std::exp((mu - 0.5 * sigma * sigma) * dt +
										 sigma * Y.at(j) * std::sqrt(dt));
				if (change < 1) {
					std::cout << "ovde sam" << std::endl;
					std::cout << change << std::endl;
				}
				prices.at(i).at(j).at(k) =
					prices.at(i).at(j).at(k - 1) * change;
			}
		}
	}
	return prices;
}
