// SPDX-License-Identifier: GPL-3.0-or-later
#include "ltspice_import.hpp"

#include <cctype>
#include <cstdlib>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace ltspice_import {

namespace {

std::vector<std::string> tokens(std::string_view s) {
    std::vector<std::string> out;
    std::size_t i = 0;
    while (i < s.size()) {
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
        std::size_t st = i;
        while (i < s.size() && !std::isspace(static_cast<unsigned char>(s[i]))) ++i;
        if (i > st) out.emplace_back(s.substr(st, i - st));
    }
    return out;
}

long long parse_int(std::string_view s) { return std::atol(std::string(s).c_str()); }

std::string trim(std::string_view s) {
    std::size_t a = 0, b = s.size();
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return std::string(s.substr(a, b - a));
}

} // namespace

std::optional<File> parse(std::string_view text) {
    File f;
    Symbol * cur = nullptr;
    std::size_t start = 0;

    while (start < text.size()) {
        auto nl = text.find('\n', start);
        std::string_view line = text.substr(start, nl == std::string_view::npos ? text.size() - start : nl - start);
        start = (nl == std::string_view::npos) ? text.size() : nl + 1;
        while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) line.remove_suffix(1);
        if (line.empty()) continue;

        auto tk = tokens(line);
        if (tk.empty()) continue;
        if      (tk[0] == "Version" && tk.size() >= 2)  f.version = std::atoi(tk[1].c_str());
        else if (tk[0] == "SHEET"   && tk.size() >= 4)  { f.sheet_w = parse_int(tk[2]); f.sheet_h = parse_int(tk[3]); }
        else if (tk[0] == "WIRE"    && tk.size() >= 5)  { Wire w{parse_int(tk[1]), parse_int(tk[2]), parse_int(tk[3]), parse_int(tk[4])}; f.wires.push_back(w); }
        else if (tk[0] == "SYMBOL"  && tk.size() >= 5)  {
            Symbol s;
            s.type = tk[1];
            s.x = parse_int(tk[2]);
            s.y = parse_int(tk[3]);
            s.rotation = tk[4];
            f.symbols.push_back(std::move(s));
            cur = &f.symbols.back();
        } else if (tk[0] == "SYMATTR" && cur) {
            if (tk.size() >= 3) {
                std::string key = tk[1];
                std::string val;
                for (std::size_t i = 2; i < tk.size(); ++i) { if (i > 2) val += ' '; val += tk[i]; }
                if      (key == "InstName") cur->inst_name = val;
                else if (key == "Value")    cur->value     = val;
                else cur->attrs.emplace_back(std::move(key), std::move(val));
            }
        } else if (tk[0] == "FLAG" && tk.size() >= 4) {
            Flag fl{ parse_int(tk[1]), parse_int(tk[2]), tk[3] };
            f.flags.push_back(fl);
        }
    }
    return f;
}

} // namespace ltspice_import
