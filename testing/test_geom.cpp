// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/345_geom.

#include "test_runner.hpp"
#include "../modules/345_geom/geom.hpp"

#include <cmath>

namespace {

testing::TestOutcome run() {
    using namespace geom;

    // Vector arithmetic.
    VECTOR2I a{100, 200}, b{50, 25};
    if ((a + b) != VECTOR2I{150, 225}) return testing::fail("vector +");
    if ((a - b) != VECTOR2I{ 50, 175}) return testing::fail("vector -");
    if (std::abs(VECTOR2I{3,4}.norm() - 5.0) > 1e-9) return testing::fail("norm");

    // Box operations.
    BOX2I box(VECTOR2I{0,0}, VECTOR2I{100,50});
    if (!box.contains(VECTOR2I{50,25})) return testing::fail("box contains");
    if ( box.contains(VECTOR2I{101,0})) return testing::fail("box contains false");
    if (box.width() != 100 || box.height() != 50) return testing::fail("box dims");

    BOX2I box2(VECTOR2I{50,25}, VECTOR2I{200,100});
    if (!box.intersects(box2)) return testing::fail("box intersects");

    // Segment distance / nearest.
    SEG seg{VECTOR2I{0,0}, VECTOR2I{100,0}};
    if (std::abs(seg.distance(VECTOR2I{50, 10}) - 10.0) > 0.5) return testing::fail("seg distance");
    auto near = seg.nearest(VECTOR2I{50, 10});
    if (std::abs(near.x - 50) > 0.5 || std::abs(near.y) > 0.5) return testing::fail("seg nearest");

    // Segment intersect.
    auto ix = SEG{VECTOR2I{0,0},   VECTOR2I{100,100}}
                .intersect(SEG{VECTOR2I{0,100}, VECTOR2I{100,0}});
    if (!ix) return testing::fail("intersect null");
    if (std::abs(ix->x - 50) > 1.0 || std::abs(ix->y - 50) > 1.0)
        return testing::fail("intersect coord");

    // Angle.
    EDA_ANGLE r90{90};
    auto rotated = rotate(VECTOR2D{100, 0}, r90);
    if (std::abs(rotated.x) > 1e-6 || std::abs(rotated.y - 100) > 1e-6)
        return testing::fail("rotate 90");
    if (EDA_ANGLE{-90}.normalized().deg() != 270.0) return testing::fail("angle norm");

    // Poly contains.
    SHAPE_LINE_CHAIN square{{0,0},{100,0},{100,100},{0,100}};
    square.set_closed(true);
    if (!square.contains(VECTOR2I{50,50})) return testing::fail("poly in");
    if ( square.contains(VECTOR2I{150,50})) return testing::fail("poly out");

    // Format.
    if (format_mm(mm_to_nm(12.7)) != "12.7") return testing::fail("format_mm");
    if (format_mm(0)              != "0")    return testing::fail("format_mm 0");
    if (format_mm(mm_to_nm(-1.5)) != "-1.5") return testing::fail("format_mm neg");

    return testing::ok();
}

const int _r = testing::register_test(
    "geom",
    "Geometry kernel: vectors, boxes, segments, angles, polylines, mm formatting.",
    &run);

} // namespace
