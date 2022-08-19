#include "../include/css-color-parser-cpp/csscolorparser.hpp"
#include <iostream>

int main() {
	auto color = CSSColorParser::parse("#f5e342ff");
	std::cout << color.uint32() << "\n";
	std::cout << color.rgbaString() << "\n";
}