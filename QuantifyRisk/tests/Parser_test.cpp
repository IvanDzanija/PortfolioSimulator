#include "CSV_parser.h"
#include "Parser_test.h"
#include "Portfolio.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

Portfolio parser_test(void) {
	CSV_Parser parser;

	std::string folder_path = "../datasets/";
	Portfolio test;
	for (const auto &entry : fs::directory_iterator(folder_path)) {
		if (entry.is_regular_file()) {
			std::cout << "Parsing file: " << entry.path().filename()
					  << std::endl;

			Cryptocurrency crypto =
				parser.fastReadCryptoCSV(fs::absolute(entry.path()).string());
			test.add_asset(crypto, 1);
		}
	}

	return test;
}
