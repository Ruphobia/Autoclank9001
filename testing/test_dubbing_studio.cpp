// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/831_dubbing_studio/dubbing_studio.hpp"

namespace {

testing::TestOutcome run_dubbing_studio() {
    dubbing_studio::init();
    auto s = dubbing_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dubbing_studio::shutdown();
    return testing::ok();
}

const int _reg_dubbing_studio = testing::register_test(
    "dubbing_studio",
    "831_dubbing_studio: stub status check",
    &run_dubbing_studio);

}
