#include "DateTime_formatting.h"
#include "Portfolio.h"
#include "math/numerical.h"
#include <algorithm>

Doubles_Matrix Portfolio::aligned_log_returns() {
	if (log_returns_aligned) {
		return aligned_log_return_matrix;
	}

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

	return math::matrix_transpose(ret);
}

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
