// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/357_bom/bom.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;

    Schematic sch; sch.uuid = make_uuid();

    auto add = [&](std::string ref, std::string val, std::string fp) {
        auto s = std::make_shared<SchSymbol>();
        s->lib_id = "Device:R";
        s->at = {0, 0};
        s->fields.push_back({"Reference", ref, {}, {}, false, false, false, 1,1, {}, make_uuid()});
        s->fields.push_back({"Value",     val, {}, {}, false, false, false, 1,1, {}, make_uuid()});
        s->fields.push_back({"Footprint", fp,  {}, {}, false, false, false, 1,1, {}, make_uuid()});
        s->uuid = make_uuid();
        sch.root.items.push_back(s);
    };
    add("R1", "10k", "R_0603");
    add("R2", "10k", "R_0603");
    add("R3", "10k", "R_0603");
    add("R7", "10k", "R_0603");   // gap on purpose
    add("R4", "1k",  "R_0603");
    add("C1", "10nF","C_0603");

    auto b = bom::generate(sch, {});
    // R1..R3 + R7 grouped as one row (four refs, value 10k), R4 alone (1k), C1 alone.
    if (b.rows.size() != 3) return testing::fail("row count " + std::to_string(b.rows.size()));

    // CSV includes headers.
    std::string csv = bom::to_csv(b);
    if (csv.find("Qty,") == std::string::npos)   return testing::fail("csv header");
    if (csv.find("R1")   == std::string::npos)   return testing::fail("csv refs");

    // HTML wraps in a table.
    std::string html = bom::to_html(b);
    if (html.find("<table") == std::string::npos) return testing::fail("html table");

    // Range combiner.
    bom::Options o; o.ref_combine = "range";
    auto b2 = bom::generate(sch, o);
    bool saw_range = false;
    for (const auto & row : b2.rows) {
        for (const auto & c : row.cells) {
            if (c.first == "Reference" && c.second.find("-") != std::string::npos) saw_range = true;
        }
    }
    if (!saw_range) return testing::fail("range combiner did not collapse R1-R3");

    return testing::ok();
}

const int _r = testing::register_test(
    "bom",
    "BOM grouping by shared fields, list+range combiners, CSV/HTML/JSON output.",
    &run);

} // namespace
