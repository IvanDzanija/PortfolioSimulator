#include "Portfolio.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;
// Definicija pybind11 modula 'montecarlo'
PYBIND11_MODULE(montecarlo, m) {
	m.doc() = "Monte Carlo simulator modul (C++ uz pybind11)";
}
