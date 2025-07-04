#include "Cryptocurrency.h"
#include "DateTime_formatting.h"
#include "Portfolio.h"
#include "math/numerical.h"
#include "math/stochastic.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <random>

std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
std::vector<Doubles_Matrix> Portfolio::monte_carlo(int simulations, int steps,
                                                   timestamp start) {
    Doubles_Matrix covariance = calculate_covariance(start);
    if (covariance.empty()) {
        std::cerr << "Starting date mismatch. Some assets probably didn't "
                     "exist as of set starting date!"
                  << std::endl;
        return {};
    }
    size_t n = covariance.size();               // number of assets
    size_t timeframe = covariance.at(0).size(); // number of candles

    // Gather the starting prices for all assets
    std::vector<double> starting_prices(n);
    std::vector<double> weights(n);
    size_t index = 0;
    for (const auto &iter : _assets) {
        const Cryptocurrency &crypto = iter.first;
        weights.at(index) = iter.second;

        // we check till aligned_end because start can be in the future and
        // gives pointer past-the-end
        std::cout << aligned_end << ' ' << crypto.name << std::endl;
        auto it = std::lower_bound(crypto.hist_data.begin(),
                                   crypto.hist_data.end(), aligned_end,
                                   [](const Candle &candle, timestamp stamp) {
                                       return candle.time < stamp;
                                   });
        it->print();
        starting_prices.at(index) = it->close;
        //	std::cout << "Starting price for " << crypto.name << ": "
        //			  << starting_prices.at(index) << std::endl;
        ++index;
    }

    //	for (size_t i = 0; i < n; ++i) {
    //		std::cout << starting_prices.at(i) << std::endl;
    //	}

    std::vector<Doubles_Matrix> Z(
        simulations, Doubles_Matrix(steps, std::vector<double>(n)));

    for (size_t i = 0; i < simulations; ++i) {
        for (size_t j = 0; j < steps; ++j) {
            for (size_t k = 0; k < n; ++k) {
                Z.at(i).at(j).at(k) =
                    std::normal_distribution<double>(0.0, 1.0)(rnd);
            }
        }
    }
    Doubles_Matrix L(n, std::vector<double>(n, 0));
    if (math::cholesky_decomposition(covariance, L) != 0) {
        std::cerr << "Cholesky decomposition failed!" << std::endl;
        return {};
    }

    // Generate correlated shocks
    std::vector<Doubles_Matrix> Y(
        simulations, Doubles_Matrix(steps, std::vector<double>(n)));
    for (size_t i = 0; i < simulations; ++i) {
        for (size_t j = 0; j < steps; ++j) {
            Y.at(i).at(j) = math::correlated_shocks(L, Z.at(i).at(j));
        }
    }

    std::vector<Doubles_Matrix> prices(
        simulations, Doubles_Matrix(n, std::vector<double>(steps)));

    double dt = 1.0;
    for (size_t i = 0; i < simulations; ++i) {
        std::cout << "Simulation " << i + 1 << " of " << simulations
                  << std::endl;
        for (size_t j = 0; j < n; ++j) {

            double mu = aligned_means.at(j);
            double sigma = aligned_volatilities.at(j);

            prices.at(i).at(j).at(0) = starting_prices.at(j) * weights.at(j);

            for (size_t k = 1; k < steps; ++k) {
                double change =
                    std::exp((mu - 0.5 * sigma * sigma) * dt +
                             sigma * Y.at(i).at(k).at(j) * std::sqrt(dt));
                prices.at(i).at(j).at(k) =
                    prices.at(i).at(j).at(k - 1) * change;
            }
            std::cout << "Finished: " << steps << " steps of asset: " << j + 1
                      << " of " << n << std::endl;
        }
    }
    return prices;
}
