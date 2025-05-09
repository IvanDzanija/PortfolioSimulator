#include "Alignment_test.h"
#include "CSV_parser.h"
#include <filesystem>
#include <iostream>
#include <map>
namespace fs = std::filesystem;

Portfolio alignment_test(void) {
	CSV_Parser parser;

	std::string folder_path = "../datasets/";
	std::map<timestamp, int> timestamps_map;

	Portfolio test;
	for (const auto &entry : fs::directory_iterator(folder_path)) {
		if (entry.is_regular_file()) {
			Cryptocurrency crypto =
				parser.fastReadCryptoCSV(fs::absolute(entry.path()).string());

			test.add_asset(crypto, 1);
		}
	}
	Doubles_Matrix aligned = test.aligned_log_returns();
	std::cout << "Aligned data:" << std::endl;
	for (const auto &row : aligned) {
		for (const auto &value : row) {
			std::cout << value << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << aligned.size() << " x " << aligned[0].size() << std::endl;

	return test;
}
