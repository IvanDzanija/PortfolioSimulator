#include "CSV_parser.h"
#include "Portfolio.h"
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(PortfolioSimulator, m) {
	m.doc() = "Portfolio simulator modul (C++ uz pybind11)";
	py::class_<Candle>(m, "Candle")
		.def_readonly("open", &Candle::open)
		.def_readonly("high", &Candle::high)
		.def_readonly("low", &Candle::low)
		.def_readonly("close", &Candle::close)
		.def_readonly("volume", &Candle::volume)
		.def_readonly("marketcap", &Candle::marketcap)
		.def_readonly("timestamp", &Candle::timestamp);

	py::class_<Cryptocurrency>(m, "Cryptocurrency")
		.def(py::init<>())
		.def("get_name", &Cryptocurrency::get_name)
		.def("get_tick", &Cryptocurrency::get_tick)
		.def("calculate_metrics", &Cryptocurrency::calculate_metrics)
		.def("individual_monte_carlo", &Cryptocurrency::individual_monte_carlo)
		.def("__eq__", &Cryptocurrency::operator==)
		.def("__hash__",
			 [](const Cryptocurrency &c) { return Cryptocurrency::Hash{}(c); });

	py::class_<CSV_Parser>(m, "CSV_Parser")
		.def(py::init<>())
		.def("fastReadCryptoCSV", &CSV_Parser::fastReadCryptoCSV);

	py::class_<Portfolio>(m, "Portfolio")
		.def(py::init<>())
		.def("add_asset", &Portfolio::add_asset)
		.def("remove_asset", &Portfolio::remove_asset)
		.def("calculate_aligned_metrics", &Portfolio::calculate_aligned_metrics)
		.def("calculate_covariance", &Portfolio::calculate_covariance,
			 py::return_value_policy::reference)
		.def("aligned_log_returns", &Portfolio::aligned_log_returns,
			 py::return_value_policy::reference)
		.def("monte_carlo", &Portfolio::monte_carlo,
			 py::return_value_policy::reference);
}
