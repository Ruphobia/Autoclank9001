// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/743_prayer_and_liturgy_translator/prayer_and_liturgy_translator.hpp"

namespace {

testing::TestOutcome run_prayer_and_liturgy_translator() {
    prayer_and_liturgy_translator::init();
    auto s = prayer_and_liturgy_translator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    prayer_and_liturgy_translator::shutdown();
    return testing::ok();
}

const int _reg_prayer_and_liturgy_translator = testing::register_test(
    "prayer_and_liturgy_translator",
    "743_prayer_and_liturgy_translator: stub status check",
    &run_prayer_and_liturgy_translator);

}
