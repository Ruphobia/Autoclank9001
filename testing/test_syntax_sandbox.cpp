// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/809_syntax_sandbox/syntax_sandbox.hpp"

namespace {

testing::TestOutcome run_syntax_sandbox() {
    syntax_sandbox::init();
    auto s = syntax_sandbox::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    syntax_sandbox::shutdown();
    return testing::ok();
}

const int _reg_syntax_sandbox = testing::register_test(
    "syntax_sandbox",
    "809_syntax_sandbox: stub status check",
    &run_syntax_sandbox);

}
