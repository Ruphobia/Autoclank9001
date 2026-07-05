// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/483_queueing_analyzer/queueing_analyzer.hpp"

namespace {

testing::TestOutcome run_queueing_analyzer() {
    queueing_analyzer::init();
    auto s = queueing_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    queueing_analyzer::shutdown();
    return testing::ok();
}

const int _reg_queueing_analyzer = testing::register_test(
    "queueing_analyzer",
    "483_queueing_analyzer: stub status check",
    &run_queueing_analyzer);

}
