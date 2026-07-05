// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/549_credibility_and_experience_rating_tool/credibility_and_experience_rating_tool.hpp"

namespace {

testing::TestOutcome run_credibility_and_experience_rating_tool() {
    credibility_and_experience_rating_tool::init();
    auto s = credibility_and_experience_rating_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    credibility_and_experience_rating_tool::shutdown();
    return testing::ok();
}

const int _reg_credibility_and_experience_rating_tool = testing::register_test(
    "credibility_and_experience_rating_tool",
    "549_credibility_and_experience_rating_tool: stub status check",
    &run_credibility_and_experience_rating_tool);

}
