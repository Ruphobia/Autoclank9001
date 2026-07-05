// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/767_atu_motif_index_browser/atu_motif_index_browser.hpp"

namespace {

testing::TestOutcome run_atu_motif_index_browser() {
    atu_motif_index_browser::init();
    auto s = atu_motif_index_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    atu_motif_index_browser::shutdown();
    return testing::ok();
}

const int _reg_atu_motif_index_browser = testing::register_test(
    "atu_motif_index_browser",
    "767_atu_motif_index_browser: stub status check",
    &run_atu_motif_index_browser);

}
