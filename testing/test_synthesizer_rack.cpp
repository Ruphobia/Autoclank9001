// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1730_synthesizer_rack/synthesizer_rack.hpp"

namespace {

testing::TestOutcome run_synthesizer_rack() {
    synthesizer_rack::init();
    auto s = synthesizer_rack::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    synthesizer_rack::shutdown();
    return testing::ok();
}

const int _reg_synthesizer_rack = testing::register_test(
    "synthesizer_rack",
    "1730_synthesizer_rack: stub status check",
    &run_synthesizer_rack);

}
