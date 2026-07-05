// SPDX-License-Identifier: GPL-3.0-or-later
#include "orcad_import.hpp"

#include <cctype>
#include <sstream>
#include <string>

namespace orcad_import {

namespace {

std::string trim(std::string_view s) {
    std::size_t a = 0, b = s.size();
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return std::string(s.substr(a, b - a));
}

// Strip surrounding single quotes.
std::string unquote(std::string_view s) {
    if (s.size() >= 2 && s.front() == '\'' && s.back() == '\'') return std::string(s.substr(1, s.size() - 2));
    return std::string(s);
}

// Split a line on whitespace but keep single-quoted tokens intact.
std::vector<std::string> tokens(std::string_view line) {
    std::vector<std::string> out;
    std::size_t i = 0;
    while (i < line.size()) {
        while (i < line.size() && std::isspace(static_cast<unsigned char>(line[i]))) ++i;
        if (i >= line.size()) break;
        if (line[i] == '\'') {
            std::size_t j = i + 1;
            while (j < line.size() && line[j] != '\'') ++j;
            out.emplace_back(line.substr(i, (j < line.size() ? j - i + 1 : line.size() - i)));
            i = j < line.size() ? j + 1 : line.size();
        } else if (line[i] == ':') {
            out.emplace_back(":");
            ++i;
        } else {
            std::size_t j = i;
            while (j < line.size() && !std::isspace(static_cast<unsigned char>(line[j])) && line[j] != '\'' && line[j] != ':') ++j;
            out.emplace_back(line.substr(i, j - i));
            i = j;
        }
    }
    return out;
}

} // namespace

std::optional<File> parse(std::string_view text) {
    File f;
    std::string section;
    Net * current_net = nullptr;

    std::size_t start = 0;
    while (start < text.size()) {
        auto nl = text.find('\n', start);
        std::string_view line_raw = text.substr(start, nl == std::string_view::npos ? text.size() - start : nl - start);
        start = (nl == std::string_view::npos) ? text.size() : nl + 1;
        std::string line = trim(line_raw);
        if (line.empty() || line == "END.") { current_net = nullptr; continue; }
        if (line.rfind("NETLIST", 0) == 0) continue;
        if (line[0] == '{' || line[0] == '#') continue;

        // Uppercase section markers.
        if (line == "COMPONENTS")   { section = "COMPONENTS"; current_net = nullptr; continue; }
        if (line == "NETS")         { section = "NETS";       current_net = nullptr; continue; }

        auto tk = tokens(line);
        if (section == "COMPONENTS") {
            // 'R1' 'RES_0603' 'R'
            if (tk.size() >= 2) {
                Component c;
                c.ref     = unquote(tk[0]);
                c.package = unquote(tk[1]);
                c.value   = tk.size() >= 3 ? unquote(tk[2]) : c.package;
                f.components.push_back(std::move(c));
            }
        } else if (section == "NETS") {
            // First token: 'name'  then ':'  then pins.
            std::size_t i = 0;
            if (!tk.empty() && tk[0][0] == '\'') {
                Net n; n.name = unquote(tk[0]);
                f.nets.push_back(std::move(n));
                current_net = &f.nets.back();
                i = 1;
            }
            if (!current_net) continue;
            if (i < tk.size() && tk[i] == ":") ++i;
            // Remaining pairs: 'R1'.'2' pattern; may appear across
            // continuation lines.
            for (; i < tk.size(); ++i) {
                std::string t = tk[i];
                if (t.empty()) continue;
                std::string ref, pin;
                auto dot = t.find('.');
                if (dot == std::string::npos) continue;
                ref = unquote(t.substr(0, dot));
                pin = unquote(t.substr(dot + 1));
                current_net->pins.push_back({ ref, pin });
            }
        }
    }
    return f;
}

} // namespace orcad_import
