// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/776_iconography_tagger/iconography_tagger.hpp"

namespace {

testing::TestOutcome run_iconography_tagger() {
    iconography_tagger::init();
    auto s = iconography_tagger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    iconography_tagger::shutdown();
    return testing::ok();
}

const int _reg_iconography_tagger = testing::register_test(
    "iconography_tagger",
    "776_iconography_tagger: stub status check",
    &run_iconography_tagger);

}
