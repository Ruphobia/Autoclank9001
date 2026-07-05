// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/354_file_carver/file_carver.hpp"

namespace {

testing::TestOutcome run_file_carver() {
    file_carver::init();
    auto s = file_carver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    file_carver::shutdown();
    return testing::ok();
}

const int _reg_file_carver = testing::register_test(
    "file_carver",
    "354_file_carver: stub status check",
    &run_file_carver);

}
