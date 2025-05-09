#ifndef PORTFOLIO_H
#define PORTFOLIO_H
#include "Cryptocurrency.h"
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <unordered_map>

using AssetAmount = boost::multiprecision::cpp_dec_float_50;
using Doubles_Matrix = std::vector<std::vector<double>>;
using Crypto_Matrix = std::vector<std::vector<Cryptocurrency>>;

class Portfolio {
  public:
	// maybe add Names and IDS in the future
	std::unordered_map<Cryptocurrency, AssetAmount, Cryptocurrency::Hash>
		assets;

	// Covariance matrix
	Doubles_Matrix aligned_log_return_matrix;
	Doubles_Matrix covariance_matrix;
	std::vector<double> aligned_means;

	bool log_returns_aligned = false;
	bool covariance_matrix_calculated = false;
	bool aligned_means_calculated = false;

	void add_asset(const Cryptocurrency &crypto, AssetAmount ammount) {
		assets.emplace(crypto, ammount);
		log_returns_aligned = false;
		covariance_matrix_calculated = false;
	}

	void remove_asset(const Cryptocurrency &crypto, AssetAmount ammount) {
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
			log_returns_aligned = false;
			covariance_matrix_calculated = false;
		}
	}

	Doubles_Matrix &aligned_log_returns();
	Doubles_Matrix &calculate_covariance();
	std::vector<double> &calculate_aligned_means();
};

#endif // PORTFOLIO_H
