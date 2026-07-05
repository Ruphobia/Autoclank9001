// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1517_morphometric_and_aging_tool/morphometric_and_aging_tool.hpp"

namespace {

testing::TestOutcome run_morphometric_and_aging_tool() {
    morphometric_and_aging_tool::init();
    auto s = morphometric_and_aging_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    morphometric_and_aging_tool::shutdown();
    return testing::ok();
}

const int _reg_morphometric_and_aging_tool = testing::register_test(
    "morphometric_and_aging_tool",
    "1517_morphometric_and_aging_tool: stub status check",
    &run_morphometric_and_aging_tool);

}
