// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/737_theological_lexicon/theological_lexicon.hpp"

namespace {

testing::TestOutcome run_theological_lexicon() {
    theological_lexicon::init();
    auto s = theological_lexicon::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    theological_lexicon::shutdown();
    return testing::ok();
}

const int _reg_theological_lexicon = testing::register_test(
    "theological_lexicon",
    "737_theological_lexicon: stub status check",
    &run_theological_lexicon);

}
