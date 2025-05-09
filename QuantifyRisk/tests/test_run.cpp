#include "Alignment_test.h"
#include "Parser_test.h"
#include <iostream>

int main(void) {
	std::cout << "Running tests..." << std::endl;

	std::cout << "Parser test:" << std::endl;
	parser_test();
	std::cout << "Alignment test:" << std::endl;
	alignment_test();
	return 0;
}
