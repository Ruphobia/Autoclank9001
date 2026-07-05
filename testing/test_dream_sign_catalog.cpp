// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2154_dream_sign_catalog/dream_sign_catalog.hpp"

namespace {

testing::TestOutcome run_dream_sign_catalog() {
    dream_sign_catalog::init();
    auto s = dream_sign_catalog::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dream_sign_catalog::shutdown();
    return testing::ok();
}

const int _reg_dream_sign_catalog = testing::register_test(
    "dream_sign_catalog",
    "2154_dream_sign_catalog: stub status check",
    &run_dream_sign_catalog);

}
