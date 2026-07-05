// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/788_morphology_glosser/morphology_glosser.hpp"

namespace {

testing::TestOutcome run_morphology_glosser() {
    morphology_glosser::init();
    auto s = morphology_glosser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    morphology_glosser::shutdown();
    return testing::ok();
}

const int _reg_morphology_glosser = testing::register_test(
    "morphology_glosser",
    "788_morphology_glosser: stub status check",
    &run_morphology_glosser);

}
