// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1531_lithic_typology/lithic_typology.hpp"

namespace {

testing::TestOutcome run_lithic_typology() {
    lithic_typology::init();
    auto s = lithic_typology::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lithic_typology::shutdown();
    return testing::ok();
}

const int _reg_lithic_typology = testing::register_test(
    "lithic_typology",
    "1531_lithic_typology: stub status check",
    &run_lithic_typology);

}
