// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1627_background_remover/background_remover.hpp"

namespace {

testing::TestOutcome run_background_remover() {
    background_remover::init();
    auto s = background_remover::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    background_remover::shutdown();
    return testing::ok();
}

const int _reg_background_remover = testing::register_test(
    "background_remover",
    "1627_background_remover: stub status check",
    &run_background_remover);

}
