// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1901_aftercare_protocol_generator/aftercare_protocol_generator.hpp"

namespace {

testing::TestOutcome run_aftercare_protocol_generator() {
    aftercare_protocol_generator::init();
    auto s = aftercare_protocol_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    aftercare_protocol_generator::shutdown();
    return testing::ok();
}

const int _reg_aftercare_protocol_generator = testing::register_test(
    "aftercare_protocol_generator",
    "1901_aftercare_protocol_generator: stub status check",
    &run_aftercare_protocol_generator);

}
