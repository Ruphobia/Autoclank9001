// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1547_plankton_micrograph_classifier/plankton_micrograph_classifier.hpp"

namespace {

testing::TestOutcome run_plankton_micrograph_classifier() {
    plankton_micrograph_classifier::init();
    auto s = plankton_micrograph_classifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    plankton_micrograph_classifier::shutdown();
    return testing::ok();
}

const int _reg_plankton_micrograph_classifier = testing::register_test(
    "plankton_micrograph_classifier",
    "1547_plankton_micrograph_classifier: stub status check",
    &run_plankton_micrograph_classifier);

}
