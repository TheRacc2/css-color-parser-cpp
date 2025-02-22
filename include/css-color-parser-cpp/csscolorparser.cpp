// (c) Dean McNamee <dean@gmail.com>, 2012.
// C++ port by Mapbox, Konstantin Käfer <mail@kkaefer.com>, 2014-2017.
//
// https://github.com/deanm/css-color-parser-js
// https://github.com/kkaefer/css-color-parser-cpp
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include "csscolorparser.hpp"

#include <cstdint>
#include <vector>
#include <sstream>
#include <algorithm>

namespace CSSColorParser {

// http://www.w3.org/TR/css3-color/
struct NamedColor { const char *const name; const Color color; };
const NamedColor namedColors[] = {
    { "transparent", { 0, 0, 0, 0 } }, { "aliceblue", { 240, 248, 255, 255 } },
    { "antiquewhite", { 250, 235, 215, 255 } }, { "aqua", { 0, 255, 255, 255 } },
    { "aquamarine", { 127, 255, 212, 255 } }, { "azure", { 240, 255, 255, 255 } },
    { "beige", { 245, 245, 220, 255 } }, { "bisque", { 255, 228, 196, 255 } },
    { "black", { 0, 0, 0, 255 } }, { "blanchedalmond", { 255, 235, 205, 255 } },
    { "blue", { 0, 0, 255, 255 } }, { "blueviolet", { 138, 43, 226, 255 } },
    { "brown", { 165, 42, 42, 255 } }, { "burlywood", { 222, 184, 135, 255 } },
    { "cadetblue", { 95, 158, 160, 255 } }, { "chartreuse", { 127, 255, 0, 255 } },
    { "chocolate", { 210, 105, 30, 255 } }, { "coral", { 255, 127, 80, 255 } },
    { "cornflowerblue", { 100, 149, 237, 255 } }, { "cornsilk", { 255, 248, 220, 255 } },
    { "crimson", { 220, 20, 60, 255 } }, { "cyan", { 0, 255, 255, 255 } },
    { "darkblue", { 0, 0, 139, 255 } }, { "darkcyan", { 0, 139, 139, 255 } },
    { "darkgoldenrod", { 184, 134, 11, 255 } }, { "darkgray", { 169, 169, 169, 255 } },
    { "darkgreen", { 0, 100, 0, 255 } }, { "darkgrey", { 169, 169, 169, 255 } },
    { "darkkhaki", { 189, 183, 107, 255 } }, { "darkmagenta", { 139, 0, 139, 255 } },
    { "darkolivegreen", { 85, 107, 47, 255 } }, { "darkorange", { 255, 140, 0, 255 } },
    { "darkorchid", { 153, 50, 204, 255 } }, { "darkred", { 139, 0, 0, 255 } },
    { "darksalmon", { 233, 150, 122, 255 } }, { "darkseagreen", { 143, 188, 143, 255 } },
    { "darkslateblue", { 72, 61, 139, 255 } }, { "darkslategray", { 47, 79, 79, 255 } },
    { "darkslategrey", { 47, 79, 79, 255 } }, { "darkturquoise", { 0, 206, 209, 255 } },
    { "darkviolet", { 148, 0, 211, 255 } }, { "deeppink", { 255, 20, 147, 255 } },
    { "deepskyblue", { 0, 191, 255, 255 } }, { "dimgray", { 105, 105, 105, 255 } },
    { "dimgrey", { 105, 105, 105, 255 } }, { "dodgerblue", { 30, 144, 255, 255 } },
    { "firebrick", { 178, 34, 34, 255 } }, { "floralwhite", { 255, 250, 240, 255 } },
    { "forestgreen", { 34, 139, 34, 255 } }, { "fuchsia", { 255, 0, 255, 255 } },
    { "gainsboro", { 220, 220, 220, 255 } }, { "ghostwhite", { 248, 248, 255, 255 } },
    { "gold", { 255, 215, 0, 255 } }, { "goldenrod", { 218, 165, 32, 255 } },
    { "gray", { 128, 128, 128, 255 } }, { "green", { 0, 128, 0, 255 } },
    { "greenyellow", { 173, 255, 47, 255 } }, { "grey", { 128, 128, 128, 255 } },
    { "honeydew", { 240, 255, 240, 255 } }, { "hotpink", { 255, 105, 180, 255 } },
    { "indianred", { 205, 92, 92, 255 } }, { "indigo", { 75, 0, 130, 255 } },
    { "ivory", { 255, 255, 240, 255 } }, { "khaki", { 240, 230, 140, 255 } },
    { "lavender", { 230, 230, 250, 255 } }, { "lavenderblush", { 255, 240, 245, 255 } },
    { "lawngreen", { 124, 252, 0, 255 } }, { "lemonchiffon", { 255, 250, 205, 255 } },
    { "lightblue", { 173, 216, 230, 255 } }, { "lightcoral", { 240, 128, 128, 255 } },
    { "lightcyan", { 224, 255, 255, 255 } }, { "lightgoldenrodyellow", { 250, 250, 210, 255 } },
    { "lightgray", { 211, 211, 211, 255 } }, { "lightgreen", { 144, 238, 144, 255 } },
    { "lightgrey", { 211, 211, 211, 255 } }, { "lightpink", { 255, 182, 193, 255 } },
    { "lightsalmon", { 255, 160, 122, 255 } }, { "lightseagreen", { 32, 178, 170, 255 } },
    { "lightskyblue", { 135, 206, 250, 255 } }, { "lightslategray", { 119, 136, 153, 255 } },
    { "lightslategrey", { 119, 136, 153, 255 } }, { "lightsteelblue", { 176, 196, 222, 255 } },
    { "lightyellow", { 255, 255, 224, 255 } }, { "lime", { 0, 255, 0, 255 } },
    { "limegreen", { 50, 205, 50, 255 } }, { "linen", { 250, 240, 230, 255 } },
    { "magenta", { 255, 0, 255, 255 } }, { "maroon", { 128, 0, 0, 255 } },
    { "mediumaquamarine", { 102, 205, 170, 255 } }, { "mediumblue", { 0, 0, 205, 255 } },
    { "mediumorchid", { 186, 85, 211, 255 } }, { "mediumpurple", { 147, 112, 219, 255 } },
    { "mediumseagreen", { 60, 179, 113, 255 } }, { "mediumslateblue", { 123, 104, 238, 255 } },
    { "mediumspringgreen", { 0, 250, 154, 255 } }, { "mediumturquoise", { 72, 209, 204, 255 } },
    { "mediumvioletred", { 199, 21, 133, 255 } }, { "midnightblue", { 25, 25, 112, 255 } },
    { "mintcream", { 245, 255, 250, 255 } }, { "mistyrose", { 255, 228, 225, 255 } },
    { "moccasin", { 255, 228, 181, 255 } }, { "navajowhite", { 255, 222, 173, 255 } },
    { "navy", { 0, 0, 128, 255 } }, { "oldlace", { 253, 245, 230, 255 } },
    { "olive", { 128, 128, 0, 255 } }, { "olivedrab", { 107, 142, 35, 255 } },
    { "orange", { 255, 165, 0, 255 } }, { "orangered", { 255, 69, 0, 255 } },
    { "orchid", { 218, 112, 214, 255 } }, { "palegoldenrod", { 238, 232, 170, 255 } },
    { "palegreen", { 152, 251, 152, 255 } }, { "paleturquoise", { 175, 238, 238, 255 } },
    { "palevioletred", { 219, 112, 147, 255 } }, { "papayawhip", { 255, 239, 213, 255 } },
    { "peachpuff", { 255, 218, 185, 255 } }, { "peru", { 205, 133, 63, 255 } },
    { "pink", { 255, 192, 203, 255 } }, { "plum", { 221, 160, 221, 255 } },
    { "powderblue", { 176, 224, 230, 255 } }, { "purple", { 128, 0, 128, 255 } },
    { "red", { 255, 0, 0, 255 } }, { "rosybrown", { 188, 143, 143, 255 } },
    { "royalblue", { 65, 105, 225, 255 } }, { "saddlebrown", { 139, 69, 19, 255 } },
    { "salmon", { 250, 128, 114, 255 } }, { "sandybrown", { 244, 164, 96, 255 } },
    { "seagreen", { 46, 139, 87, 255 } }, { "seashell", { 255, 245, 238, 255 } },
    { "sienna", { 160, 82, 45, 255 } }, { "silver", { 192, 192, 192, 255 } },
    { "skyblue", { 135, 206, 235, 255 } }, { "slateblue", { 106, 90, 205, 255 } },
    { "slategray", { 112, 128, 144, 255 } }, { "slategrey", { 112, 128, 144, 255 } },
    { "snow", { 255, 250, 250, 255 } }, { "springgreen", { 0, 255, 127, 255 } },
    { "steelblue", { 70, 130, 180, 255 } }, { "tan", { 210, 180, 140, 255 } },
    { "teal", { 0, 128, 128, 255 } }, { "thistle", { 216, 191, 216, 255 } },
    { "tomato", { 255, 99, 71, 255 } }, { "turquoise", { 64, 224, 208, 255 } },
    { "violet", { 238, 130, 238, 255 } }, { "wheat", { 245, 222, 179, 255 } },
    { "white", { 255, 255, 255, 255 } }, { "whitesmoke", { 245, 245, 245, 255 } },
    { "yellow", { 255, 255, 0, 255 } }, { "yellowgreen", { 154, 205, 50, 255 } }
};

template <typename T>
uint8_t clamp_css_byte(T i) {  // Clamp to integer 0 .. 255.
    i = ::round(i);  // Seems to be what Chrome does (vs truncation).
    return i < 0 ? 0 : i > 255 ? 255 : uint8_t(i);
}

template <typename T>
float clamp_css_float(T f) {  // Clamp to float 0.0 .. 255.0
    return f < 0 ? 0 : f > 1 ? 1 : float(f);
}

float parseFloat(const std::string& str) {
    return strtof(str.c_str(), nullptr);
}

int64_t parseInt(const std::string& str, uint8_t base = 10) {
    return strtoll(str.c_str(), nullptr, base);
}

uint8_t parse_css_int(const std::string& str) {  // int or percentage.
    if (str.length() && str.back() == '%') {
        return clamp_css_byte(parseFloat(str) / 100.0f * 255.0f);
    } else {
        return clamp_css_byte(parseInt(str));
    }
}

float parse_css_float(const std::string& str) {  // float or percentage.
    if (str.length() && str.back() == '%') {
        return clamp_css_float(parseFloat(str) / 100.0f);
    } else {
        return clamp_css_float(parseFloat(str));
    }
}

uint8_t parse_better(const std::string& str) {
    if (str.find('.') != std::string::npos)
        return parse_css_float(str) * 255;
    else
        return parse_css_int(str);
}

float css_hue_to_rgb(float m1, float m2, float h) {
    if (h < 0.0f) {
        h += 1.0f;
    } else if (h > 1.0f) {
        h -= 1.0f;
    }

    if (h * 6.0f < 1.0f) {
        return m1 + (m2 - m1) * h * 6.0f;
    }
    if (h * 2.0f < 1.0f) {
        return m2;
    }
    if (h * 3.0f < 2.0f) {
        return m1 + (m2 - m1) * (2.0f / 3.0f - h) * 6.0f;
    }
    return m1;
}



std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

Color parse(const std::string& css_str) {
    std::string str = css_str;

    // Remove all whitespace, not compliant, but should just be more accepting.
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());

    // Convert to lowercase.
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    for (const auto& namedColor : namedColors) {
        if (str == namedColor.name) {
            return { namedColor.color };
        }
    }

    // #abc and #abc123 syntax.
    if (str.length() && str.front() == '#') {
        int64_t iv = 0;
        switch (str.length()) {
            case 4:
                iv = parseInt(str.substr(1), 16);  // TODO(deanm): Stricter parsing.
                if (!(iv >= 0 && iv <= 0xfff)) {
                    return {};
                }
                else {
                    return { 
                        static_cast<uint8_t>(((iv & 0xf00) >> 4) | ((iv & 0xf00) >> 8)),
                        static_cast<uint8_t>((iv & 0xf0) | ((iv & 0xf0) >> 4)),
                        static_cast<uint8_t>((iv & 0xf) | ((iv & 0xf) << 4)),
                        1
                    };
                }
                break;

            case 7:
                iv = parseInt(str.substr(1), 16);  // TODO(deanm): Stricter parsing.
                if (!(iv >= 0 && iv <= 0xffffff)) {
                    return {};  // Covers NaN.
                }
                else {
                    return {
                        static_cast<uint8_t>((iv & 0xff0000) >> 16),
                        static_cast<uint8_t>((iv & 0xff00) >> 8),
                        static_cast<uint8_t>(iv & 0xff),
                        1
                    };
                }
                break;

            case 9:
                iv = parseInt(str.substr(1), 16);  // theracc2: custom 8 digit hex
                if (!(iv >= 0 && iv <= 0xffffffff)) {
                    return {};  // Covers NaN.
                }
                else {
                    return {
                        static_cast<uint8_t>((iv & 0xff000000) >> 24),
                        static_cast<uint8_t>((iv & 0xff0000) >> 16),
                        static_cast<uint8_t>((iv & 0xff00) >> 8), 
                        static_cast<uint8_t>(iv & 0xff)
                    };
                }
                break;
        }

        return {};
    }

    size_t op = str.find_first_of('('), ep = str.find_first_of(')');
    if (op != std::string::npos && ep + 1 == str.length()) {
        const std::string fname = str.substr(0, op);
        const std::vector<std::string> params = split(str.substr(op + 1, ep - (op + 1)), ',');

        unsigned char alpha = 255;
        
        if (fname == "rgba" || fname == "rgb") {
            if (fname == "rgba") {
                if (params.size() != 4) {
                    return {};
                }
                alpha = parse_better(params.back());
            } else {
                if (params.size() != 3) {
                    return {};
                }
            }

            return {
                parse_better(params[0]),
                parse_better(params[1]),
                parse_better(params[2]),
                alpha
            };

        } else if (fname == "hsla" || fname == "hsl") {
            if (fname == "hsla") {
                if (params.size() != 4) {
                    return {};
                }
                alpha = parse_better(params.back());
            } else {
                if (params.size() != 3) {
                    return {};
                }
            }

            float h = parseFloat(params[0]) / 360.0f;
            float i;
            // Normalize the hue to [0..1[
            h = std::modf(h, &i);

            // NOTE(deanm): According to the CSS spec s/l should only be
            // percentages, but we don't bother and let float or percentage.
            float s = parse_css_float(params[1]);
            float l = parse_css_float(params[2]);

            float m2 = l <= 0.5f ? l * (s + 1.0f) : l + s - l * s;
            float m1 = l * 2.0f - m2;

            return {
                clamp_css_byte(css_hue_to_rgb(m1, m2, h + 1.0f / 3.0f) * 255.0f),
                clamp_css_byte(css_hue_to_rgb(m1, m2, h) * 255.0f),
                clamp_css_byte(css_hue_to_rgb(m1, m2, h - 1.0f / 3.0f) * 255.0f),
                alpha
            };
        }
    }

    return {};
}

} // namespace CSSColorParser
