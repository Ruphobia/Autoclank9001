// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/339_transpiler/transpiler.hpp"

namespace {

testing::TestOutcome run_transpiler() {
    transpiler::init();
    auto s = transpiler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    transpiler::shutdown();
    return testing::ok();
}

const int _reg_transpiler = testing::register_test(
    "transpiler",
    "339_transpiler: stub status check",
    &run_transpiler);

}
