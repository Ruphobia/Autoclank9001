// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/341_kicad_libs.

#include "test_runner.hpp"
#include "../modules/341_kicad_libs/kicad_libs.hpp"

#include <string>

namespace {

testing::TestOutcome run() {
    kicad_libs::init();
    const auto & cfg = kicad_libs::config();

    // Ready state depends on where the libs are installed. When neither
    // symbol nor footprint roots are populated we skip rather than fail:
    // MVP scaffold; the roadmap adds a vendored corpus.
    if (!cfg.ready) {
        return testing::skip(
            "no symbol/footprint libs found; set TOOL_KICAD_SYMBOL_ROOT / "
            "TOOL_KICAD_FOOTPRINT_ROOT, or install kicad-symbols");
    }

    // Symbol search: 555 is the poster child.
    auto syms = kicad_libs::search_symbols("NE555", 5);
    if (syms.empty()) syms = kicad_libs::search_symbols("555", 5);
    // Footprint search: DIP-8 is universal.
    auto fps  = kicad_libs::search_footprints("DIP-8", 5);
    if (fps.empty()) fps = kicad_libs::search_footprints("DIP", 5);

    if (syms.empty() && fps.empty()) {
        return testing::fail(
            "libraries indexed but neither '555'/'NE555' nor 'DIP-8'/'DIP' "
            "matched; symbol_libs=" + std::to_string(cfg.symbol_libs) +
            " footprint_libs=" + std::to_string(cfg.footprint_libs));
    }

    kicad_libs::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "kicad_libs",
    "Index available KiCad symbol and footprint libraries; search for a "
    "canonical part.",
    &run);

} // namespace
