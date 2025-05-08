#include "Monte_Carlo.h"
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <chrono>
#include <exception>
#include <pybind11/pybind11.h>
#include <random>
#include <unordered_map>

namespace py = pybind11;

using AssetAmount = boost::multiprecision::cpp_dec_float_50;

struct Candle {
	// HLOC
	double high, low, open, close;
	double volume, marketcap;
	std::chrono::sys_seconds timestamp;

	double log_return;
	Candle() {}
};

class CorruptData : public std::exception {
  private:
	std::string message;

  public:
	CorruptData(const std::string &message) : message(message) {}

	const char *what() const noexcept override { return message.c_str(); }
};

class Cryptocurrency {
	// this class needs to provide default hashing
  public:
	std::string name, tick;
	std::vector<Candle> hist_data;
	bool metrics_calculated = false;

	// Drift = sum(return_i) / n;
	// Volatility = sqrt((1/n - 1) * sum((return_i - Drift)^2)
	double drift, volatility;

	void calculate_metrics() {
		if (metrics_calculated)
			return;

		double sum = 0, sum_sq = 0;
		int n = 0;

		for (const Candle &c : hist_data) {
			if (std::isnan(c.log_return))
				// shouldn't ever get here
				throw CorruptData("Log return is NaN!");
			sum += c.log_return;
			sum_sq += c.log_return * c.log_return;
			++n;
		}
		if (n > 1) {
			drift = sum / n;
			volatility = std::sqrt((sum_sq - n * drift * drift) / (n - 1));
		}
		metrics_calculated = true;
		return;
	}
	void reevaluate_metrics() {
		metrics_calculated = false;
		return;
	}

	void individual_monte_carlo(int sim_count, int forecast_days) {

		std::default_random_engine gen(std::random_device{}());
		std::normal_distribution<double> dist(0.0, 1.0);

		this->calculate_metrics();
		double S0 = this->hist_data.back().close;
		double mu = this->drift;
		double sigma = this->volatility;

		std::vector<std::vector<double>> simulations(sim_count);

		for (int i = 0; i < sim_count; ++i) {
			double price = S0;
			for (int j = 0; j < forecast_days; ++j) {
				double Z = dist(gen);
				price =
					price * std::exp((mu - 0.5 * sigma * sigma) + sigma * Z);
				simulations[i].push_back(price);
			}
		}
	}
};

using Doubles_Matrix = std::vector<std::vector<double>>;
using Crypto_Matrix = std::vector<std::vector<Cryptocurrency>>;
class Portfolio {
  public:
	// maybe add Names and IDS in the future
	std::unordered_map<Cryptocurrency, AssetAmount> assets;

	// Covariance matrix
	Doubles_Matrix covariance_matrix;
	bool covariance_matrix_calculated = false;

	void add_asset(const Cryptocurrency &crypto, AssetAmount ammount) {
		assets.emplace(crypto, ammount);
	}

	Crypto_Matrix align_dates() {
		// O(n^2) is best possible ?
		Crypto_Matrix ret;
		std::vector<size_t> last_positions;
		std::chrono::sys_seconds skip_to{};
		const Cryptocurrency &pivot = assets.begin()->first;
		for (const Candle &candle : pivot.hist_data) {
			std::chrono::sys_seconds stamp = candle.timestamp;
			if (stamp < skip_to) {
				continue;
			}
			auto iter = std::next(assets.begin());

			std::vector<Cryptocurrency> possible;
			for (; iter != assets.end(); iter = std::next(iter)) {
				for (const Candle &candle : iter->first.hist_data) {
					if (candle.timestamp > stamp) {
						skip_to = candle.timestamp;
						break;
					} else if (candle.timestamp == stamp) {
						possible.push_back(iter->first);
					}
				}
			}
			if (possible.size() == assets.size()) {
				ret.push_back(possible);
			}
		}
		return ret;
	}

	void calculate_covariance() { Doubles_Matrix aligned = align_dates(); }
};

// Implementacija Monte Carlo simulacije
double run_simulation(int num_paths, int steps) {
	// Inicijaliziraj generator slučajnih brojeva
	std::mt19937 rng(12345);
	std::normal_distribution<double> dist(0.0, 1.0);

	double sum_final = 0.0;
	// Pokreni 'num_paths' simulacija
	for (int i = 0; i < num_paths; i++) {
		double value = 0.0;
		// Svaki put računaj zbroj normalno distribuiranih koraka
		for (int j = 0; j < steps; j++) {
			value += dist(rng);
		}
		sum_final += value;
	}
	// Vrati prosjek
	return sum_final / num_paths;
}
int main(void) { return 0; }

// Definicija pybind11 modula 'montecarlo'
PYBIND11_MODULE(montecarlo, m) {
	m.doc() = "Monte Carlo simulator modul (C++ uz pybind11)";

	// Registriraj funkciju run_simulation u Python
	m.def("run_simulation", &run_simulation, "Pokreće Monte Carlo simulaciju",
		  py::arg("num_paths") = 10000, // zadane vrijednosti
		  py::arg("steps") = 100);
}
