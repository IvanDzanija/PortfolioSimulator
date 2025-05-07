#include "Monte_Carlo.h"
#include <pybind11/pybind11.h>
#include <random>

namespace py = pybind11;

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
