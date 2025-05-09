#include "CSV_parser.h"
#include "Portfolio.h"
#include <exception>
#include <iostream>
#include <pybind11/pybind11.h>
#include <random>

namespace py = pybind11;

class CorruptData : public std::exception {
  private:
	std::string message;

  public:
	CorruptData(const std::string &message) : message(message) {}

	const char *what() const noexcept override { return message.c_str(); }
};

void Cryptocurrency::calculate_metrics() {
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
void Cryptocurrency::reevaluate_metrics() {
	metrics_calculated = false;
	return;
}

void Cryptocurrency::individual_monte_carlo(int sim_count, int forecast_days) {

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
			price = price * std::exp((mu - 0.5 * sigma * sigma) + sigma * Z);
			simulations[i].push_back(price);
		}
	}
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

// Definicija pybind11 modula 'montecarlo'
PYBIND11_MODULE(montecarlo, m) {
	m.doc() = "Monte Carlo simulator modul (C++ uz pybind11)";

	// Registriraj funkciju run_simulation u Python
	m.def("run_simulation", &run_simulation, "Pokreće Monte Carlo simulaciju",
		  py::arg("num_paths") = 10000, // zadane vrijednosti
		  py::arg("steps") = 100);
}
