// SPDX-License-Identifier: GPL-3.0-or-later
#include "gerber_viewer_native.hpp"

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>

namespace gerber_viewer_native {

namespace {

struct State {
    int  int_digits = 4;
    int  dec_digits = 6;
    bool leading_zero_supp = true;
    int  current_ap = -1;
    geom::VECTOR2I xy{0,0};
    // Aperture id -> diameter (in the gerber's units, converted to nm).
    std::unordered_map<int, long long> ap_diam_nm;
    bool metric = true;
};

// Parse a coordinate value using the format spec.
long long parse_coord(std::string_view s, int int_digits, int dec_digits, bool metric) {
    // Values arrive as integer strings; place a decimal point at
    // `dec_digits` from the right, then convert to nm.
    if (s.empty()) return 0;
    long long sign = 1;
    std::size_t i = 0;
    if (s[0] == '-') { sign = -1; i = 1; }
    else if (s[0] == '+') { i = 1; }
    std::string digits(s.substr(i));
    // Pad with leading zeros to (int_digits + dec_digits).
    std::size_t total = static_cast<std::size_t>(int_digits + dec_digits);
    while (digits.size() < total) digits = "0" + digits;
    // integer.decimal split.
    std::string ints = digits.substr(0, digits.size() - dec_digits);
    std::string decs = digits.substr(digits.size() - dec_digits);
    double v = std::atof((ints + "." + decs).c_str()) * sign;
    return metric
        ? geom::mm_to_nm(v)
        : geom::mm_to_nm(v * 25.4);
}

std::string_view chomp_until(std::string_view text, std::size_t & pos, char stop) {
    std::size_t s = pos;
    while (pos < text.size() && text[pos] != stop) ++pos;
    return text.substr(s, pos - s);
}

} // namespace

Parsed parse(std::string_view text) {
    Parsed out;
    State st;
    std::size_t p = 0;
    auto extend = [&](geom::VECTOR2I v) { out.bbox.extend(v); };

    while (p < text.size()) {
        char c = text[p];
        if (c == '\n' || c == '\r' || c == ' ' || c == '\t') { ++p; continue; }

        if (c == '%') {
            // Extended command block.
            ++p;
            auto cmd = chomp_until(text, p, '%');
            if (p < text.size()) ++p;

            // FSLAXaYb or FSTAXaYb.
            if (cmd.rfind("FS", 0) == 0) {
                st.leading_zero_supp = cmd.size() > 2 && cmd[2] == 'L';
                auto xpos = cmd.find('X');
                auto ypos = cmd.find('Y');
                if (xpos != std::string_view::npos && xpos + 2 < cmd.size()) {
                    st.int_digits = cmd[xpos + 1] - '0';
                    st.dec_digits = cmd[xpos + 2] - '0';
                }
                (void) ypos;
            } else if (cmd.rfind("MO", 0) == 0) {
                st.metric = (cmd.find("MM") != std::string_view::npos);
            } else if (cmd.rfind("ADD", 0) == 0) {
                // ADD<num>C,dia or ADD<num>R,wxh or ADD<num>O,wxh.
                std::size_t i = 3;
                std::string num;
                while (i < cmd.size() && std::isdigit(static_cast<unsigned char>(cmd[i]))) num += cmd[i++];
                if (num.empty() || i >= cmd.size()) continue;
                int id = std::atoi(num.c_str());
                char kind = cmd[i++];
                if (i < cmd.size() && cmd[i] == ',') ++i;
                std::string tail(cmd.substr(i));
                // Extract first double.
                double d = std::atof(tail.c_str());
                long long dia = st.metric
                    ? geom::mm_to_nm(d)
                    : geom::mm_to_nm(d * 25.4);
                (void) kind;
                st.ap_diam_nm[id] = dia;
            }
            continue;
        }
        // Non-% command: read up to '*'.
        std::size_t start = p;
        while (p < text.size() && text[p] != '*') ++p;
        std::string_view line(text.substr(start, p - start));
        if (p < text.size()) ++p;

        // Skip Gcodes we don't need behavior for.
        if (line.rfind("G", 0) == 0 && line.size() > 1 && std::isdigit(static_cast<unsigned char>(line[1]))) {
            // G01 / G02 / G03 / G36 / G37 / G54 / G75 : we only care to
            // ignore for MVP; extend later for arcs.
            continue;
        }
        // D-code: change aperture.
        if (line.rfind("D", 0) == 0 && line.size() > 1 &&
            std::isdigit(static_cast<unsigned char>(line[1]))) {
            int id = std::atoi(line.data() + 1);
            if (id >= 10) st.current_ap = id;
            // D01/D02/D03 come with X/Y prefixes; those are handled below.
        }
        // XY line: parse X<coord>Y<coord>D0N.
        long long nx = st.xy.x, ny = st.xy.y;
        int op = 0;   // 1 = draw, 2 = move, 3 = flash
        for (std::size_t k = 0; k < line.size(); ++k) {
            char cc = line[k];
            if (cc == 'X' || cc == 'Y') {
                std::size_t s = k + 1;
                std::size_t e = s;
                while (e < line.size() &&
                       (std::isdigit(static_cast<unsigned char>(line[e])) ||
                        line[e] == '-' || line[e] == '+' || line[e] == '.')) ++e;
                long long v = parse_coord(line.substr(s, e - s),
                                          st.int_digits, st.dec_digits, st.metric);
                if (cc == 'X') nx = v; else ny = v;
                k = e - 1;
            }
            if (cc == 'D' && k + 3 <= line.size()) {
                int d = std::atoi(line.data() + k + 1);
                if (d == 1) op = 1;
                else if (d == 2) op = 2;
                else if (d == 3) op = 3;
            }
        }
        if (op == 1) {
            out.lines.emplace_back(st.xy, geom::VECTOR2I{nx, ny});
            extend(st.xy); extend({nx, ny});
        } else if (op == 3) {
            long long dia = 0;
            auto it = st.ap_diam_nm.find(st.current_ap);
            if (it != st.ap_diam_nm.end()) dia = it->second;
            out.flashes.emplace_back(geom::VECTOR2I{nx, ny}, dia);
            extend({nx - dia/2, ny - dia/2});
            extend({nx + dia/2, ny + dia/2});
        }
        if (op) st.xy = { nx, ny };
    }
    out.ok = !out.lines.empty() || !out.flashes.empty();
    return out;
}

std::string to_svg(const Parsed & p, const Options & opts) {
    std::ostringstream os;
    if (!p.ok || !p.bbox.valid()) {
        os << "<svg xmlns='http://www.w3.org/2000/svg' width='300' height='60'>"
              "<text x='4' y='30' fill='#c00'>Empty gerber</text></svg>";
        return os.str();
    }
    double pad = opts.pad_mm;
    double lo_x = geom::nm_to_mm(p.bbox.lo.x) - pad;
    double lo_y = geom::nm_to_mm(p.bbox.lo.y) - pad;
    double w    = geom::nm_to_mm(p.bbox.hi.x - p.bbox.lo.x) + 2 * pad;
    double h    = geom::nm_to_mm(p.bbox.hi.y - p.bbox.lo.y) + 2 * pad;
    // Y-invert to match KiCad convention (gerber Y is up).
    os << "<svg xmlns='http://www.w3.org/2000/svg' viewBox='"
       << lo_x << " " << -(lo_y + h) << " " << w << " " << h
       << "' width='800' height='" << (800.0 * h / w) << "'>\n";
    os << "  <g transform='scale(1,-1)' fill='" << opts.fill_color
       << "' stroke='" << opts.stroke_color << "' stroke-linecap='round'>\n";
    for (const auto & kv : p.lines) {
        os << "    <line x1='" << geom::nm_to_mm(kv.first.x)
           << "' y1='" << geom::nm_to_mm(kv.first.y)
           << "' x2='" << geom::nm_to_mm(kv.second.x)
           << "' y2='" << geom::nm_to_mm(kv.second.y)
           << "' stroke-width='0.15'/>\n";
    }
    for (const auto & f : p.flashes) {
        double d = geom::nm_to_mm(f.second);
        if (d <= 0) d = 0.2;
        os << "    <circle cx='" << geom::nm_to_mm(f.first.x)
           << "' cy='" << geom::nm_to_mm(f.first.y)
           << "' r='" << (d / 2)
           << "' stroke='none'/>\n";
    }
    os << "  </g>\n</svg>\n";
    return os.str();
}

std::string render_to_svg(std::string_view t, const Options & opts) {
    return to_svg(parse(t), opts);
}

} // namespace gerber_viewer_native
