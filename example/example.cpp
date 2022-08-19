#include "../include/css-color-parser-cpp/csscolorparser.hpp"
#include <iostream>

int main() {
	auto color = CSSColorParser::parse("#f5e342ff");
	std::cout << color.value().uint32() << "\n";
	std::cout << color.value().rgbaString() << "\n";
}