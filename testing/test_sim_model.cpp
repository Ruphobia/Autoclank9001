// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/375_sim_model/sim_model.hpp"

namespace {

testing::TestOutcome run() {
    using namespace sim_model;

    // Catalog basics.
    auto cat = catalog();
    if (cat.size() < 5) return testing::fail("catalog size");

    // Emit resistor.
    Model r; r.kind = Kind::Resistor; r.ref = "R1"; r.value = "10k";
    std::string s = to_spice(r);
    if (s.find("R1 10k") == std::string::npos) return testing::fail("R emit");

    // Emit diode with .MODEL block.
    Model d = catalog()[5];
    std::string ds = to_spice(d);
    if (ds.find(".MODEL DMOD D") == std::string::npos) return testing::fail("D model");
    if (ds.find("IS=1e-14") == std::string::npos) return testing::fail("D params");

    // JSON round-trip.
    std::string j = to_json(d);
    Model d2 = from_json(j);
    if (d2.model_name != d.model_name) return testing::fail("json model_name");
    if (d2.params.size() != d.params.size()) return testing::fail("json params");

    // Parse a raw SPICE line.
    auto p = parse_spice_line("R2 a b 4.7k");
    if (!p) return testing::fail("parse fail");
    if (p->kind != Kind::Resistor || p->ref != "R2" || p->value != "4.7k")
        return testing::fail("parse fields");
    return testing::ok();
}

const int _r = testing::register_test(
    "sim_model",
    "SIM_MODEL: catalog defaults, SPICE emitters for R/C/L/D/Q/M, JSON round-trip, SPICE-line parser.",
    &run);

} // namespace
