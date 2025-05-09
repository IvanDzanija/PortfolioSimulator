#ifndef PORTFOLIO_H
#define PORTFOLIO_H
#include "Cryptocurrency.h"
#include "DateTime_formatting.h"
#include <algorithm>
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

	bool log_returns_aligned = false;
	bool covariance_matrix_calculated = false;

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

	Doubles_Matrix aligned_log_returns() {
		if (log_returns_aligned) {
			return aligned_log_return_matrix;
		}
		// O(n^2) is best possible ?
		Doubles_Matrix ret;
		std::vector<size_t> last_positions(assets.size() - 1, 0);
		timestamp skip_to{};
		const Cryptocurrency &pivot = assets.begin()->first;
		for (size_t index = 0; index < pivot.hist_data.size(); ++index) {
			const Candle &candle_pivot = pivot.hist_data.at(index);
			timestamp stamp = candle_pivot.timestamp;

			if (stamp < skip_to) {
				auto skip = std::lower_bound(
					pivot.hist_data.begin(), pivot.hist_data.end(), skip_to,
					[](const Candle &candle, timestamp stamp) {
						return candle.timestamp < stamp;
					});
				if (skip != pivot.hist_data.end()) {
					index = std::distance(pivot.hist_data.begin(), skip);
				} else {
					// No more candles to process, break the loop
					break;
				}
				continue;
			}

			// create a vector with full size and fill in the pivot
			std::vector<double> possible(assets.size());
			possible.at(0) = candle_pivot.log_return;

			// loop over all the assets
			// vector last_positions is used to skip the candles that are
			// already processed
			size_t asset_index = 0;
			bool skip_assets = false;
			for (auto iter = std::next(assets.begin());
				 !skip_assets && iter != assets.end(); iter = std::next(iter)) {

				for (size_t i = last_positions.at(asset_index);
					 i < iter->first.hist_data.size(); ++i) {
					const Candle &candle = iter->first.hist_data.at(i);

					if (candle.timestamp > stamp) {
						skip_to = candle.timestamp;
						last_positions.at(asset_index) = i + 1;
						// this skips all the other assets since this date is no
						// good
						skip_assets = true;
						break;
					} else if (candle.timestamp == stamp) {
						possible.at(asset_index + 1) = candle.log_return;
						last_positions.at(asset_index) = i + 1;
						// we update here in case we don't find the last candle
						// updating after will think we found
						++asset_index;
						break;
					} else {
						// this is the case when we have a candle that is
						// older than the pivot candle
						// we can skip this candle and move to the next one
						continue;
					}
				}
				if (skip_assets) {
					break;
				}
			}
			if (asset_index + 1 == assets.size()) {
				ret.push_back(possible);
			}
		}
		log_returns_aligned = true;
		return ret;
	}

	Doubles_Matrix calculate_covariance();
};

#endif // PORTFOLIO_H
