// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/890_detector_noise_modeler/detector_noise_modeler.hpp"

namespace {

testing::TestOutcome run_detector_noise_modeler() {
    detector_noise_modeler::init();
    auto s = detector_noise_modeler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    detector_noise_modeler::shutdown();
    return testing::ok();
}

const int _reg_detector_noise_modeler = testing::register_test(
    "detector_noise_modeler",
    "890_detector_noise_modeler: stub status check",
    &run_detector_noise_modeler);

}
