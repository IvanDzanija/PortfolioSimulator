#ifndef PORTFOLIO_H
#define PORTFOLIO_H
#include "Cryptocurrency.h"
#include <unordered_map>

using Doubles_Matrix = std::vector<std::vector<double>>;
using Crypto_Matrix = std::vector<std::vector<Cryptocurrency>>;

class Portfolio {
  public:
	// maybe add Names and IDS in the future
	std::unordered_map<Cryptocurrency, double, Cryptocurrency::Hash> assets;

	// Covariance matrix
	Doubles_Matrix aligned_log_return_matrix;
	Doubles_Matrix covariance_matrix;
	std::vector<double> aligned_means;
	std::vector<double> aligned_volatilities;

	timestamp aligned_start = timestamp();
	timestamp aligned_end = timestamp();
	timestamp aligned_stamp = timestamp();
	bool aligned_returns_calculated = false;
	bool covariance_matrix_calculated = false;
	bool aligned_metrics_calculated = false;

	void add_asset(const Cryptocurrency &crypto, double ammount) {
		assets.emplace(crypto, ammount);
		aligned_returns_calculated = false;
		aligned_metrics_calculated = false;
		covariance_matrix_calculated = false;
		// if only one asset is in the portfolio we can't properly align the
		// other since the only one is the pivot so we set the aligned_start to
		// the timestamp of the first asset only if it is the only one
		if (assets.size() == 1) {
			aligned_start = crypto.hist_data.at(0).timestamp;
			aligned_end = crypto.hist_data.back().timestamp;
		} else {
			aligned_start = timestamp();
			aligned_end = timestamp();
		}
		aligned_stamp = timestamp();
	}

	void remove_asset(const Cryptocurrency &crypto, double ammount) {
		auto it = assets.find(crypto);
		if (it != assets.end()) {
			if (it->second >= ammount) {
				it->second -= ammount;
				if (it->second == 0) {
					assets.erase(it);
				}
			} else {
				std::cerr << "Not enough asset to remove" << std::endl;
			}
			// same as in add_asset
			if (assets.size() == 1) {
				aligned_start = crypto.hist_data.at(0).timestamp;
				aligned_end = crypto.hist_data.back().timestamp;
			} else {
				aligned_start = timestamp();
				aligned_end = timestamp();
			}
			aligned_stamp = timestamp();
			aligned_returns_calculated = false;
			aligned_metrics_calculated = false;
			covariance_matrix_calculated = false;
		}
	}

	Cryptocurrency get_asset(const std::string &name) {
		for (auto &pair : assets) {
			if (pair.first.get_name() == name) {
				return pair.first;
			}
		}
		throw std::runtime_error("Asset not found");
	}

	Doubles_Matrix &aligned_log_returns(timestamp start);
	Doubles_Matrix &calculate_covariance(timestamp start);
	int calculate_aligned_metrics(timestamp start);
	std::vector<Doubles_Matrix> monte_carlo(int simulations, int steps,
											timestamp start);
};

#endif // PORTFOLIO_H
