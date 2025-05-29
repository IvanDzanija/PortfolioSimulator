#ifndef PORTFOLIO_H
#define PORTFOLIO_H
#include "Cryptocurrency.h"
#include <unordered_map>

using Doubles_Matrix = std::vector<std::vector<double>>;
using Crypto_Matrix = std::vector<std::vector<Cryptocurrency>>;

class Portfolio {
  private:
    std::vector<std::string> _asset_names;
    std::unordered_map<Cryptocurrency, double, Cryptocurrency::Hash> _assets;

  public:
    // maybe add Names and IDS in the future

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
        _assets.emplace(crypto, ammount);
        aligned_returns_calculated = false;
        aligned_metrics_calculated = false;
        covariance_matrix_calculated = false;
        // if only one asset is in the portfolio we can't properly align the
        // other since the only one is the pivot so we set the aligned_start to
        // the timestamp of the first asset only if it is the only one
        if (_assets.size() == 1) {
            aligned_start = crypto.hist_data.at(0).time;
            aligned_end = crypto.hist_data.back().time;
        } else {
            aligned_start = timestamp();
            aligned_end = timestamp();
        }
        aligned_stamp = timestamp();
    }

    void remove_asset(const Cryptocurrency &crypto, double ammount) {
        auto it = _assets.find(crypto);
        if (it != _assets.end()) {
            if (it->second >= ammount) {
                it->second -= ammount;
                if (it->second == 0) {
                    _assets.erase(it);
                }
            } else {
                std::cerr << "Not enough asset to remove" << std::endl;
            }
            // same as in add_asset
            if (_assets.size() == 1) {
                aligned_start = crypto.hist_data.at(0).time;
                aligned_end = crypto.hist_data.back().time;
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

    Cryptocurrency get_asset(const std::string &name) const {
        for (auto &pair : _assets) {
            if (pair.first.get_name() == name) {
                return pair.first;
            }
        }
        throw std::runtime_error("Asset not found");
    }

    std::vector<std::string> &get_asset_names() {
        if (!_asset_names.empty()) {
            return _asset_names;
        }

        std::vector<std::string> names;
        for (const auto &pair : _assets) {
            names.push_back(pair.first.get_name());
        }
        _asset_names = names;
        return _asset_names;
    }

    Doubles_Matrix &aligned_log_returns(timestamp start);
    Doubles_Matrix &calculate_covariance(timestamp start);
    int calculate_aligned_metrics(timestamp start);
    std::vector<Doubles_Matrix> monte_carlo(int simulations, int steps,
                                            timestamp start);
    int PCA(timestamp start, int num_components,
            std::vector<std::pair<double, std::vector<double>>> &components,
            double &total_variance, double &variance_explained);
};

#endif // PORTFOLIO_H
