// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2145_cemetery_and_burial_mapper/cemetery_and_burial_mapper.hpp"

namespace {

testing::TestOutcome run_cemetery_and_burial_mapper() {
    cemetery_and_burial_mapper::init();
    auto s = cemetery_and_burial_mapper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cemetery_and_burial_mapper::shutdown();
    return testing::ok();
}

const int _reg_cemetery_and_burial_mapper = testing::register_test(
    "cemetery_and_burial_mapper",
    "2145_cemetery_and_burial_mapper: stub status check",
    &run_cemetery_and_burial_mapper);

}
