#include "DateTime_formatting.h"
#include "Portfolio.h"
#include "math/numerical.h"
#include <algorithm>

Doubles_Matrix &Portfolio::aligned_log_returns(timestamp start) {
	if (aligned_returns_calculated && aligned_stamp == start) {
		return aligned_log_return_matrix;
	}

	Doubles_Matrix ret;
	std::vector<size_t> last_positions(assets.size() - 1, 0);
	timestamp skip_to{};
	const Cryptocurrency &pivot = assets.begin()->first;
	for (size_t index = 0; index < pivot.hist_data.size(); ++index) {
		const Candle &candle_pivot = pivot.hist_data.at(index);
		timestamp stamp = candle_pivot.timestamp;
		if (stamp > start) {
			break;
		}

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
					if (asset_index == 0) {
						// we need to save this as the starting timestamp
						aligned_start = candle.timestamp;
					}
					++asset_index;
					// we update ending timestamp on each iteration
					aligned_end = candle.timestamp;
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
	aligned_stamp = start;
	aligned_log_return_matrix = math::matrix_transpose(ret);
	return aligned_log_return_matrix;
}

int Portfolio::calculate_aligned_metrics(timestamp start) {
	if (aligned_metrics_calculated && aligned_stamp == start) {
		return 0;
	}
	Doubles_Matrix returns = aligned_log_returns(start);
	size_t rows = returns.size();		// number of assets
	size_t cols = returns.at(0).size(); // number of candles

	aligned_means.resize(rows);
	aligned_volatilities.resize(rows);

	for (size_t i = 0; i < rows; ++i) {
		double sum = 0, sum_sq = 0;
		int n = 0;
		for (size_t j = 0; j < cols; ++j) {
			sum += returns.at(i).at(j);
			sum_sq += returns.at(i).at(j) * returns.at(i).at(j);
			++n;
		}
		if (n > 1) {
			aligned_means.at(i) = sum / n;
			aligned_volatilities.at(i) = std::sqrt(
				(sum_sq - n * aligned_means.at(i) * aligned_means.at(i)) /
				(n - 1));
		}
	}
	aligned_metrics_calculated = true;
	return 0;
}

Doubles_Matrix &Portfolio::calculate_covariance(timestamp start) {
	if (covariance_matrix_calculated && aligned_stamp == start) {
		return covariance_matrix;
	}

	Doubles_Matrix returns = aligned_log_returns(start);
	int info = calculate_aligned_metrics(start);
	if (info != 0) {
		std::cerr << "Error calculating aligned metrics" << std::endl;
		return covariance_matrix;
	}
	int rows = returns.size(); // number of assets
	Doubles_Matrix covariance(rows, std::vector<double>(rows));

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < rows; ++j) {
			covariance.at(i).at(j) =
				math::covariance(returns.at(i), aligned_means.at(i),
								 returns.at(j), aligned_means.at(j));

			// covariance is symmetric
			covariance.at(j).at(i) = covariance.at(i).at(j);
		}
	}

	covariance_matrix = std::move(covariance);
	covariance_matrix_calculated = true;
	return covariance_matrix;
}
