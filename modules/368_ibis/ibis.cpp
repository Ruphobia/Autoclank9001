// SPDX-License-Identifier: GPL-3.0-or-later
#include "ibis.hpp"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

namespace ibis {

namespace {

std::string trim(std::string_view s) {
    std::size_t a = 0, b = s.size();
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return std::string(s.substr(a, b - a));
}

// Handle IBIS's engineering suffixes: "1.2n" -> 1.2e-9, "3.3k" -> 3300.
double parse_number(std::string_view s) {
    std::string t = trim(s);
    if (t.empty() || t == "NA" || t == "na") return 0.0;
    // Strip trailing unit letters (V, s, A, ...).
    while (!t.empty() && (std::isalpha(static_cast<unsigned char>(t.back()))
                          && t.back() != 'p' && t.back() != 'n' && t.back() != 'u'
                          && t.back() != 'm' && t.back() != 'k' && t.back() != 'M'
                          && t.back() != 'G' && t.back() != 'f')) t.pop_back();
    double mul = 1.0;
    if (!t.empty()) {
        char c = t.back();
        switch (c) {
            case 'p': mul = 1e-12; t.pop_back(); break;
            case 'n': mul = 1e-9;  t.pop_back(); break;
            case 'u': mul = 1e-6;  t.pop_back(); break;
            case 'm': mul = 1e-3;  t.pop_back(); break;
            case 'k': mul = 1e3;   t.pop_back(); break;
            case 'M': mul = 1e6;   t.pop_back(); break;
            case 'G': mul = 1e9;   t.pop_back(); break;
            case 'f': mul = 1e-15; t.pop_back(); break;
        }
    }
    return std::atof(t.c_str()) * mul;
}

// Split a line by whitespace.
std::vector<std::string> tokens(std::string_view line) {
    std::vector<std::string> out;
    std::size_t i = 0;
    while (i < line.size()) {
        while (i < line.size() && std::isspace(static_cast<unsigned char>(line[i]))) ++i;
        std::size_t s = i;
        while (i < line.size() && !std::isspace(static_cast<unsigned char>(line[i]))) ++i;
        if (i > s) out.emplace_back(line.substr(s, i - s));
    }
    return out;
}

std::string_view strip_comment(std::string_view line) {
    auto p = line.find('|');   // IBIS comment character
    return p == std::string_view::npos ? line : line.substr(0, p);
}

} // namespace

std::optional<File> parse(std::string_view text) {
    File f;
    std::string current_section;
    Model * cur_model = nullptr;
    IVTable * cur_iv  = nullptr;
    VTTable * cur_vt  = nullptr;

    std::size_t start = 0;
    while (start < text.size()) {
        auto nl = text.find('\n', start);
        std::string_view line_raw = text.substr(start,
            nl == std::string_view::npos ? text.size() - start : nl - start);
        start = (nl == std::string_view::npos) ? text.size() : nl + 1;
        auto line = strip_comment(line_raw);
        std::string t = trim(line);
        if (t.empty()) continue;

        if (t[0] == '[') {
            auto rb = t.find(']');
            std::string section = trim(rb == std::string::npos ? t : t.substr(1, rb - 1));
            std::string rest    = rb == std::string::npos ? std::string{} : trim(t.substr(rb + 1));
            current_section     = section;

            if (section == "IBIS Ver") { f.header.ibis_ver = rest; }
            else if (section == "File Name")     f.header.file_name = rest;
            else if (section == "File Rev")      f.header.file_rev  = rest;
            else if (section == "Date")          f.header.date      = rest;
            else if (section == "Source")        f.header.source    = rest;
            else if (section == "Notes")         f.header.notes     = rest;
            else if (section == "Disclaimer")    f.header.disclaimer= rest;
            else if (section == "Copyright")     f.header.copyright = rest;
            else if (section == "Component")     f.header.component = rest;
            else if (section == "Manufacturer")  f.header.manufacturer = rest;
            else if (section == "Package")       {}
            else if (section == "Package Model") f.header.package_name = rest;
            else if (section == "Pin") { cur_model = nullptr; cur_iv = nullptr; cur_vt = nullptr; }
            else if (section == "Model") {
                Model m; m.name = rest;
                f.models.push_back(std::move(m));
                cur_model = &f.models.back();
                cur_iv = nullptr; cur_vt = nullptr;
            }
            else if (section == "Pulldown" || section == "Pullup" ||
                     section == "POWER Clamp" || section == "GND Clamp") {
                if (cur_model) {
                    IVTable tbl; tbl.kind = section;
                    cur_model->iv.push_back(std::move(tbl));
                    cur_iv = &cur_model->iv.back();
                    cur_vt = nullptr;
                }
            }
            else if (section == "Rising Waveform" || section == "Falling Waveform") {
                if (cur_model) {
                    VTTable tbl; tbl.kind = section;
                    cur_model->vt.push_back(std::move(tbl));
                    cur_vt = &cur_model->vt.back();
                    cur_iv = nullptr;
                }
            }
            else if (section == "End") { break; }
            continue;
        }

        if (current_section == "Pin") {
            auto tk = tokens(t);
            if (tk.size() >= 3) {
                Pin p;
                p.name        = tk[0];
                p.signal_name = tk[1];
                p.model_name  = tk[2];
                if (tk.size() >= 4) p.r_pin_ohm = parse_number(tk[3]);
                if (tk.size() >= 5) p.l_pin_nH  = parse_number(tk[4]) * 1e9;   // seconds -> nH? just carry raw
                if (tk.size() >= 6) p.c_pin_pF  = parse_number(tk[5]) * 1e12;
                f.pins.push_back(std::move(p));
            }
        } else if (current_section == "Model" && cur_model) {
            auto tk = tokens(t);
            if (tk.size() >= 2) {
                if      (tk[0] == "Model_type")   cur_model->model_type = tk[1];
                else if (tk[0] == "C_comp")       cur_model->c_comp_pF  = parse_number(tk[1]) * 1e12;
                else if (tk[0] == "Vinh")         cur_model->vinh       = parse_number(tk[1]);
                else if (tk[0] == "Vinl")         cur_model->vinl       = parse_number(tk[1]);
            }
        } else if (cur_iv) {
            auto tk = tokens(t);
            if (tk.size() >= 2) {
                double v = parse_number(tk[0]);
                double i = parse_number(tk[1]);
                cur_iv->points.emplace_back(v, i);
            }
        } else if (cur_vt) {
            auto tk = tokens(t);
            if (tk.size() >= 2) {
                double tt = parse_number(tk[0]);
                double v  = parse_number(tk[1]);
                cur_vt->points.emplace_back(tt, v);
            }
        }
    }
    return f;
}

std::optional<File> parse_file(std::string_view path) {
    std::ifstream f{std::string(path)};
    if (!f) return std::nullopt;
    std::stringstream ss; ss << f.rdbuf();
    return parse(ss.str());
}

} // namespace ibis
