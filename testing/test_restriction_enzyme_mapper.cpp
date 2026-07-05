// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1330_restriction_enzyme_mapper/restriction_enzyme_mapper.hpp"

namespace {

testing::TestOutcome run_restriction_enzyme_mapper() {
    restriction_enzyme_mapper::init();
    auto s = restriction_enzyme_mapper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    restriction_enzyme_mapper::shutdown();
    return testing::ok();
}

const int _reg_restriction_enzyme_mapper = testing::register_test(
    "restriction_enzyme_mapper",
    "1330_restriction_enzyme_mapper: stub status check",
    &run_restriction_enzyme_mapper);

}
