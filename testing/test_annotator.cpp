// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/354_annotator.

#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/354_annotator/annotator.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;

    Schematic sch;
    sch.uuid = make_uuid();

    auto make = [&](std::string ref, long long x_mm, long long y_mm, std::string val) {
        auto s = std::make_shared<SchSymbol>();
        s->lib_id = "Device:R";
        s->at = { geom::mm_to_nm(x_mm), geom::mm_to_nm(y_mm) };
        s->fields.push_back({"Reference", ref, {}, {}, false, false, false, 1, 1, {}, make_uuid()});
        s->fields.push_back({"Value",     val, {}, {}, false, false, false, 1, 1, {}, make_uuid()});
        s->uuid = make_uuid();
        sch.root.items.push_back(s);
        return s;
    };

    // Some already-annotated, some unannotated.
    make("R1", 10, 10, "10k");
    auto q1 = make("R?", 20, 10, "1k");
    auto q2 = make("R?", 30, 10, "1k");
    make("C1", 40, 10, "10uF");
    auto qc = make("C?", 50, 10, "100nF");
    auto qu = make("U?", 60, 10, "NE555");

    auto rep = annotator::annotate(sch);

    // R? entries should have become R2, R3 (or R2/R3 in some order given sort).
    auto refOf = [&](std::shared_ptr<SchSymbol> s) { return s->reference(); };
    std::string r_q1 = refOf(q1), r_q2 = refOf(q2);
    if (r_q1.substr(0,1) != "R") return testing::fail("q1 prefix");
    if (r_q2.substr(0,1) != "R") return testing::fail("q2 prefix");
    if (r_q1 == "R?" || r_q2 == "R?") return testing::fail("unannotated remains");
    if (annotator::number_of(r_q1) < 2 || annotator::number_of(r_q2) < 2)
        return testing::fail("did not skip existing R1");
    if (r_q1 == r_q2) return testing::fail("assigned duplicate");

    // C? -> C2 (C1 was in use).
    if (refOf(qc) != "C2") return testing::fail("C? did not become C2");

    // U? -> U1 (nothing else in use).
    if (refOf(qu) != "U1") return testing::fail("U? did not become U1");

    // At least three changes recorded.
    if (rep.changes.size() < 4) return testing::fail("expected >=4 changes");

    // prefix_of / number_of unit checks.
    if (annotator::prefix_of("Q7") != "Q") return testing::fail("prefix_of Q7");
    if (annotator::number_of("Q7") != 7)   return testing::fail("number_of Q7");
    if (annotator::number_of("Q?") != 0)   return testing::fail("number_of Q?");

    return testing::ok();
}

const int _r = testing::register_test(
    "annotator",
    "Refdes annotator: numbers unannotated symbols per prefix, skips gaps around used numbers.",
    &run);

} // namespace
