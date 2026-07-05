// SPDX-License-Identifier: GPL-3.0-or-later
#include "geom.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>

namespace geom {

// -------------------- SEG -------------------------

double SEG::distance(VECTOR2I p) const {
    auto n = nearest(p);
    double dx = n.x - p.x, dy = n.y - p.y;
    return std::hypot(dx, dy);
}

VECTOR2D SEG::nearest(VECTOR2I p) const {
    VECTOR2D ad{ static_cast<double>(a.x), static_cast<double>(a.y) };
    VECTOR2D bd{ static_cast<double>(b.x), static_cast<double>(b.y) };
    VECTOR2D pd{ static_cast<double>(p.x), static_cast<double>(p.y) };
    VECTOR2D ab{ bd.x - ad.x, bd.y - ad.y };
    double L2 = ab.x * ab.x + ab.y * ab.y;
    if (L2 <= 0.0) return ad;
    double t = ((pd.x - ad.x) * ab.x + (pd.y - ad.y) * ab.y) / L2;
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;
    return { ad.x + t * ab.x, ad.y + t * ab.y };
}

std::optional<VECTOR2D> SEG::intersect(const SEG & o) const {
    VECTOR2D p{ (double)a.x, (double)a.y };
    VECTOR2D r{ (double)(b.x - a.x), (double)(b.y - a.y) };
    VECTOR2D q{ (double)o.a.x, (double)o.a.y };
    VECTOR2D s{ (double)(o.b.x - o.a.x), (double)(o.b.y - o.a.y) };
    double rxs   = r.x * s.y - r.y * s.x;
    double qp_x  = q.x - p.x, qp_y = q.y - p.y;
    if (std::abs(rxs) < 1e-12) return std::nullopt; // parallel
    double t = (qp_x * s.y - qp_y * s.x) / rxs;
    double u = (qp_x * r.y - qp_y * r.x) / rxs;
    if (t < 0.0 || t > 1.0 || u < 0.0 || u > 1.0) return std::nullopt;
    return VECTOR2D{ p.x + t * r.x, p.y + t * r.y };
}

// -------------------- SHAPE_LINE_CHAIN -----------

BOX2I SHAPE_LINE_CHAIN::bbox() const {
    BOX2I b;
    for (const auto & p : m_pts) b.extend(p);
    return b;
}

bool SHAPE_LINE_CHAIN::contains(VECTOR2I p) const {
    if (m_pts.size() < 3 || !m_closed) return false;
    // Even-odd ray-cast to the right.
    bool inside = false;
    for (std::size_t i = 0, j = m_pts.size() - 1; i < m_pts.size(); j = i++) {
        const auto & a = m_pts[i];
        const auto & b = m_pts[j];
        bool crossing = ((a.y > p.y) != (b.y > p.y));
        if (crossing) {
            double xint = (double)(b.x - a.x) * (double)(p.y - a.y) /
                          (double)(b.y - a.y) + (double)a.x;
            if ((double)p.x < xint) inside = !inside;
        }
    }
    return inside;
}

double SHAPE_LINE_CHAIN::distance(VECTOR2I p) const {
    if (m_pts.empty()) return std::numeric_limits<double>::infinity();
    double best = std::numeric_limits<double>::infinity();
    std::size_t n = segment_count();
    for (std::size_t i = 0; i < n; ++i) {
        double d = segment(i).distance(p);
        if (d < best) best = d;
    }
    return best;
}

// -------------------- SHAPE_POLY_SET -------------

BOX2I SHAPE_POLY_SET::bbox() const {
    BOX2I b;
    for (const auto & p : m_polys) {
        b.merge(p.outline.bbox());
        // Holes are strictly inside; do not extend the bbox.
    }
    return b;
}

bool SHAPE_POLY_SET::contains(VECTOR2I p) const {
    for (const auto & poly : m_polys) {
        if (!poly.outline.contains(p)) continue;
        bool in_hole = false;
        for (const auto & h : poly.holes) {
            if (h.contains(p)) { in_hole = true; break; }
        }
        if (!in_hole) return true;
    }
    return false;
}

// -------------------- Formatting ------------------

std::string format_mm(long long nm) {
    double mm = nm_to_mm(nm);
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.6f", mm);
    std::string s = buf;
    if (s.find('.') != std::string::npos) {
        while (s.size() > 1 && s.back() == '0') s.pop_back();
        if (!s.empty() && s.back() == '.') s.pop_back();
    }
    if (s.empty() || s == "-0") s = "0";
    return s;
}

std::string format_mm_pair(VECTOR2I p) {
    return format_mm(p.x) + " " + format_mm(p.y);
}

} // namespace geom
