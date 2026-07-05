// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <optional>
#include <string>
#include <utility>
#include <vector>

// KiCad-equivalent geometry primitives without wx dependency.
// Coordinates default to "nm" (integer nanometers) inside the model,
// same convention as KiCad; the double variant is for math where
// integer overflow / rounding matters.
namespace geom {

// -------------------- Vector2 --------------------

template <typename T>
struct Vector2 {
    T x = 0, y = 0;

    constexpr Vector2() = default;
    constexpr Vector2(T x_, T y_) : x(x_), y(y_) {}

    constexpr Vector2 operator+(const Vector2 & o) const { return {x + o.x, y + o.y}; }
    constexpr Vector2 operator-(const Vector2 & o) const { return {x - o.x, y - o.y}; }
    constexpr Vector2 operator-() const                  { return {-x, -y}; }
    constexpr Vector2 operator*(double s) const          { return {static_cast<T>(x * s), static_cast<T>(y * s)}; }
    constexpr bool    operator==(const Vector2 & o) const { return x == o.x && y == o.y; }
    constexpr bool    operator!=(const Vector2 & o) const { return !(*this == o); }

    double norm() const   { return std::hypot(static_cast<double>(x), static_cast<double>(y)); }
    double norm2() const  { double dx = x, dy = y; return dx*dx + dy*dy; }
    Vector2<double> to_double() const { return {static_cast<double>(x), static_cast<double>(y)}; }
};

using VECTOR2I = Vector2<long long>;   // integer nanometers
using VECTOR2D = Vector2<double>;

inline VECTOR2D operator*(double s, const VECTOR2D & v) { return {s*v.x, s*v.y}; }

// -------------------- BOX2 (axis-aligned bounding box) --------------

template <typename T>
struct Box2 {
    Vector2<T> lo, hi;

    constexpr Box2() : lo{ std::numeric_limits<T>::max() / 2, std::numeric_limits<T>::max() / 2 },
                       hi{-std::numeric_limits<T>::max() / 2,-std::numeric_limits<T>::max() / 2 } {}
    constexpr Box2(Vector2<T> a, Vector2<T> b) {
        lo = { std::min(a.x, b.x), std::min(a.y, b.y) };
        hi = { std::max(a.x, b.x), std::max(a.y, b.y) };
    }

    bool     valid()  const { return lo.x <= hi.x && lo.y <= hi.y; }
    T        width()  const { return hi.x - lo.x; }
    T        height() const { return hi.y - lo.y; }
    Vector2<T> center() const { return { (lo.x + hi.x) / 2, (lo.y + hi.y) / 2 }; }

    bool contains(Vector2<T> p) const {
        return p.x >= lo.x && p.x <= hi.x && p.y >= lo.y && p.y <= hi.y;
    }
    bool intersects(const Box2 & o) const {
        return !(o.hi.x < lo.x || o.lo.x > hi.x || o.hi.y < lo.y || o.lo.y > hi.y);
    }
    Box2 & extend(Vector2<T> p) {
        if (!valid()) { lo = hi = p; return *this; }
        lo.x = std::min(lo.x, p.x); lo.y = std::min(lo.y, p.y);
        hi.x = std::max(hi.x, p.x); hi.y = std::max(hi.y, p.y);
        return *this;
    }
    Box2 & merge(const Box2 & o) {
        if (!o.valid()) return *this;
        extend(o.lo); extend(o.hi); return *this;
    }
    Box2 inflated(T d) const {
        Box2 r; r.lo = {lo.x - d, lo.y - d}; r.hi = {hi.x + d, hi.y + d}; return r;
    }
};

using BOX2I = Box2<long long>;
using BOX2D = Box2<double>;

// -------------------- Angle -----------------------

// KiCad-style EDA_ANGLE: stored as degrees but exposes radians when
// asked. Constructor accepts degrees by default; use radians() /
// deg() for conversions and normalize() to fold into [0, 360).
class EDA_ANGLE {
public:
    constexpr EDA_ANGLE() = default;
    constexpr explicit EDA_ANGLE(double deg) : m_deg(deg) {}

    static constexpr EDA_ANGLE from_deg(double d) { return EDA_ANGLE{d}; }
    static EDA_ANGLE from_rad(double r) { return EDA_ANGLE{r * 180.0 / M_PI}; }

    constexpr double deg() const { return m_deg; }
    double rad() const           { return m_deg * M_PI / 180.0; }
    double sin() const           { return std::sin(rad()); }
    double cos() const           { return std::cos(rad()); }

    EDA_ANGLE normalized() const {
        double d = std::fmod(m_deg, 360.0);
        if (d < 0) d += 360.0;
        return EDA_ANGLE{d};
    }

    constexpr EDA_ANGLE operator+(EDA_ANGLE o) const { return EDA_ANGLE{m_deg + o.m_deg}; }
    constexpr EDA_ANGLE operator-(EDA_ANGLE o) const { return EDA_ANGLE{m_deg - o.m_deg}; }
    constexpr EDA_ANGLE operator-() const            { return EDA_ANGLE{-m_deg}; }
    constexpr bool      operator==(EDA_ANGLE o) const { return m_deg == o.m_deg; }

private:
    double m_deg = 0.0;
};

inline VECTOR2D rotate(const VECTOR2D & p, EDA_ANGLE a) {
    double c = a.cos(), s = a.sin();
    return { p.x * c - p.y * s, p.x * s + p.y * c };
}
inline VECTOR2I rotate(const VECTOR2I & p, EDA_ANGLE a) {
    auto d = rotate(p.to_double(), a);
    return { static_cast<long long>(std::llround(d.x)), static_cast<long long>(std::llround(d.y)) };
}

// -------------------- SEG (line segment) ----------

struct SEG {
    VECTOR2I a, b;
    SEG() = default;
    SEG(VECTOR2I a_, VECTOR2I b_) : a(a_), b(b_) {}
    double length() const { return (b - a).norm(); }
    VECTOR2D midpoint() const {
        return { (a.x + b.x) / 2.0, (a.y + b.y) / 2.0 };
    }
    // Distance from point p to this segment.
    double distance(VECTOR2I p) const;
    // Nearest point on the segment to p.
    VECTOR2D nearest(VECTOR2I p) const;
    // Intersect with another segment; returns nullopt if parallel or non-crossing.
    std::optional<VECTOR2D> intersect(const SEG & o) const;
};

// -------------------- CIRCLE / ARC ----------------

struct CIRCLE {
    VECTOR2I center;
    long long radius = 0;

    bool contains(VECTOR2I p) const {
        auto d = (p - center);
        return d.norm2() <= double(radius) * double(radius);
    }
};

struct ARC {
    // Center-radius-angle form. `start` sweeps counter-clockwise to
    // `start + span` around `center`.
    VECTOR2I  center;
    long long radius = 0;
    EDA_ANGLE start;
    EDA_ANGLE span;
};

// -------------------- SHAPE_LINE_CHAIN -----------

// Ordered list of points forming a polyline (open) or polygon (closed).
class SHAPE_LINE_CHAIN {
public:
    SHAPE_LINE_CHAIN() = default;
    SHAPE_LINE_CHAIN(std::initializer_list<VECTOR2I> pts) : m_pts(pts) {}

    std::size_t point_count() const { return m_pts.size(); }
    const VECTOR2I & point(std::size_t i) const { return m_pts.at(i); }
    void append(VECTOR2I p) { m_pts.push_back(p); }
    void set_closed(bool c) { m_closed = c; }
    bool closed() const     { return m_closed; }

    // Segments in order. Open: (pt[0],pt[1]),(pt[1],pt[2])...; closed
    // adds the wrap segment (pt[N-1], pt[0]).
    std::size_t segment_count() const {
        if (m_pts.size() < 2) return 0;
        return m_closed ? m_pts.size() : (m_pts.size() - 1);
    }
    SEG segment(std::size_t i) const {
        std::size_t n = m_pts.size();
        return {m_pts[i], m_pts[(i + 1) % n]};
    }

    BOX2I bbox() const;

    // True when the (closed) chain contains p, via even-odd rule.
    bool contains(VECTOR2I p) const;

    // Distance from p to the chain.
    double distance(VECTOR2I p) const;

    const std::vector<VECTOR2I> & points() const { return m_pts; }

private:
    std::vector<VECTOR2I> m_pts;
    bool                  m_closed = false;
};

// -------------------- SHAPE_POLY_SET -------------

// Simplified poly-set: outer chain + holes. Full boolean ops are a
// follow-up (integrate Clipper2 when we vendor it).
class SHAPE_POLY_SET {
public:
    struct Polygon {
        SHAPE_LINE_CHAIN               outline;
        std::vector<SHAPE_LINE_CHAIN>  holes;
    };

    void add_outline(SHAPE_LINE_CHAIN o) {
        Polygon p; p.outline = std::move(o);
        p.outline.set_closed(true);
        m_polys.push_back(std::move(p));
    }
    void add_hole(std::size_t poly_idx, SHAPE_LINE_CHAIN h) {
        h.set_closed(true);
        m_polys.at(poly_idx).holes.push_back(std::move(h));
    }

    std::size_t     size()             const { return m_polys.size(); }
    const Polygon & polygon(std::size_t i) const { return m_polys.at(i); }
    Polygon &       polygon(std::size_t i)       { return m_polys.at(i); }

    BOX2I bbox() const;
    bool  contains(VECTOR2I p) const;

private:
    std::vector<Polygon> m_polys;
};

// -------------------- Formatting ------------------

// KiCad stores coordinates as integer nanometers but writes them as
// millimeter decimals. These helpers convert.
inline double nm_to_mm(long long nm)  { return static_cast<double>(nm) / 1e6; }
inline long long mm_to_nm(double mm)  {
    return static_cast<long long>(std::llround(mm * 1e6));
}

std::string format_mm(long long nm);         // "12.7"
std::string format_mm_pair(VECTOR2I p);      // "12.7 25.4"

} // namespace geom
