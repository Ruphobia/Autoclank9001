// SPDX-License-Identifier: GPL-3.0-or-later
#include "drill_viewer.hpp"

#include <cctype>
#include <cstdlib>
#include <sstream>
#include <string>
#include <unordered_map>

namespace drill_viewer {

namespace {

double parse_double(std::string_view s) { return std::strtod(std::string(s).c_str(), nullptr); }

} // namespace

Parsed parse(std::string_view text) {
    Parsed out;
    std::unordered_map<int, long long> tool_dia_nm;   // T-code -> dia_nm
    int current_tool = 0;

    std::size_t i = 0;
    while (i < text.size()) {
        std::size_t nl = text.find('\n', i);
        std::string_view line = text.substr(i, nl == std::string_view::npos ? text.size() - i : nl - i);
        i = (nl == std::string_view::npos) ? text.size() : nl + 1;

        while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) line.remove_suffix(1);
        if (line.empty() || line[0] == ';') continue;
        if (line == "M48" || line == "%" || line == "M30" || line == "G90" || line == "G05") continue;
        if (line.rfind("METRIC", 0) == 0) { out.metric = true; continue; }
        if (line.rfind("INCH",   0) == 0) { out.metric = false; continue; }
        if (line.rfind("FMAT",   0) == 0) continue;

        // Tool definitions: TxCyy.yy
        if (line[0] == 'T' && line.find('C') != std::string_view::npos) {
            std::size_t c = line.find('C');
            int t = std::atoi(line.substr(1, c - 1).data());
            double dia = parse_double(line.substr(c + 1));
            tool_dia_nm[t] = out.metric ? geom::mm_to_nm(dia)
                                        : geom::mm_to_nm(dia * 25.4);
            continue;
        }
        // Tool select: TN (no C)
        if (line[0] == 'T' && line.find('C') == std::string_view::npos) {
            current_tool = std::atoi(line.data() + 1);
            continue;
        }
        // Hole line: XnnnnYnnnn
        if (line[0] == 'X' || line[0] == 'Y') {
            long long x = 0, y = 0;
            std::size_t k = 0;
            while (k < line.size()) {
                if (line[k] == 'X' || line[k] == 'Y') {
                    char which = line[k];
                    ++k;
                    std::size_t s = k;
                    while (k < line.size() &&
                           (std::isdigit(static_cast<unsigned char>(line[k])) ||
                            line[k] == '-' || line[k] == '+' || line[k] == '.')) ++k;
                    double v = parse_double(line.substr(s, k - s));
                    if (out.metric) {
                        if (which == 'X') x = geom::mm_to_nm(v);
                        else              y = geom::mm_to_nm(-v);   // KiCad drills use Y up-positive
                    } else {
                        if (which == 'X') x = geom::mm_to_nm(v * 25.4);
                        else              y = geom::mm_to_nm(-v * 25.4);
                    }
                } else ++k;
            }
            long long dia = 0;
            auto it = tool_dia_nm.find(current_tool);
            if (it != tool_dia_nm.end()) dia = it->second;
            out.holes.push_back({ {x, y}, dia });
            out.bbox.extend({ x - dia/2, y - dia/2 });
            out.bbox.extend({ x + dia/2, y + dia/2 });
        }
    }
    out.ok = !out.holes.empty();
    return out;
}

std::string to_svg(const Parsed & p, const SvgOptions & opts) {
    std::ostringstream os;
    if (!p.ok || !p.bbox.valid()) {
        os << "<svg xmlns='http://www.w3.org/2000/svg' width='300' height='60'>"
              "<text x='4' y='30' fill='#c00'>Empty drill file</text></svg>";
        return os.str();
    }
    double pad  = opts.pad_mm;
    double lo_x = geom::nm_to_mm(p.bbox.lo.x) - pad;
    double lo_y = geom::nm_to_mm(p.bbox.lo.y) - pad;
    double w    = geom::nm_to_mm(p.bbox.hi.x - p.bbox.lo.x) + 2 * pad;
    double h    = geom::nm_to_mm(p.bbox.hi.y - p.bbox.lo.y) + 2 * pad;
    os << "<svg xmlns='http://www.w3.org/2000/svg' viewBox='"
       << lo_x << " " << lo_y << " " << w << " " << h
       << "' width='800' height='" << (800.0 * h / w) << "'>\n";
    os << "  <rect x='" << lo_x << "' y='" << lo_y << "' width='" << w
       << "' height='" << h << "' fill='" << opts.bg_color << "'/>\n";
    for (const auto & hole : p.holes) {
        double d = geom::nm_to_mm(hole.dia_nm);
        if (d <= 0) d = 0.3;
        os << "  <circle cx='" << geom::nm_to_mm(hole.at.x)
           << "' cy='" << geom::nm_to_mm(hole.at.y)
           << "' r='" << (d / 2)
           << "' fill='" << opts.hole_color << "'/>\n";
    }
    os << "</svg>\n";
    return os.str();
}

std::string render_to_svg(std::string_view t, const SvgOptions & opts) {
    return to_svg(parse(t), opts);
}

} // namespace drill_viewer
