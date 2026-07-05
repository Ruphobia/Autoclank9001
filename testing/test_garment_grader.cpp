// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1765_garment_grader/garment_grader.hpp"

namespace {

testing::TestOutcome run_garment_grader() {
    garment_grader::init();
    auto s = garment_grader::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    garment_grader::shutdown();
    return testing::ok();
}

const int _reg_garment_grader = testing::register_test(
    "garment_grader",
    "1765_garment_grader: stub status check",
    &run_garment_grader);

}
