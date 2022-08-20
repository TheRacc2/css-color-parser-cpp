#include "../include/css-color-parser-cpp/csscolorparser.hpp"
#include <iostream>

int main() {
	auto color = CSSColorParser::parse("rgba(255, 255, 255, 0.5)");
	std::cout << color.uint32() << "\n";
	std::cout << color.rgbaString() << "\n";
}