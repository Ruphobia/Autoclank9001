// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/342_kicad_project.

#include "test_runner.hpp"
#include "../modules/342_kicad_project/kicad_project.hpp"

#include <nlohmann/json.hpp>
#include <string>

namespace {

testing::TestOutcome run() {
    auto pj  = kicad_project::default_project("smoke");
    if (pj.netclasses.empty()) return testing::fail("no default netclasses");
    if (pj.netclasses[0].name != "Default") return testing::fail("first netclass must be Default");

    std::string pro = kicad_project::to_json(pj, 0);
    auto j = nlohmann::json::parse(pro, nullptr, false);
    if (j.is_discarded()) return testing::fail("emitted .kicad_pro is not valid JSON");

    // Required top-level sections KiCad expects.
    for (const char * k : {"board","boards","cvpcb","erc","libraries","meta",
                           "net_settings","pcbnew","schematic","sheets","text_variables"}) {
        if (!j.contains(k)) return testing::fail(std::string("missing top-level '") + k + "'");
    }
    if (!j["net_settings"]["classes"].is_array() || j["net_settings"]["classes"].empty())
        return testing::fail("net_settings.classes should carry at least Default");

    // ERC pin_map must be 12x12.
    const auto & pm = j["erc"]["pin_map"];
    if (!pm.is_array() || pm.size() != 12) return testing::fail("pin_map size");
    for (const auto & row : pm) if (!row.is_array() || row.size() != 12) return testing::fail("pin_map row size");

    // .kicad_prl round-trip.
    std::string prl = kicad_project::to_prl_json(pj);
    auto p = nlohmann::json::parse(prl, nullptr, false);
    if (p.is_discarded())         return testing::fail(".kicad_prl not valid JSON");
    if (!p.contains("board"))     return testing::fail(".kicad_prl missing board");

    return testing::ok();
}

const int _r = testing::register_test(
    "kicad_project",
    "Emit a JLC-friendly .kicad_pro + .kicad_prl. Verify JSON validity and required sections.",
    &run);

} // namespace
