// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/804_phoneme_inventory_designer/phoneme_inventory_designer.hpp"

namespace {

testing::TestOutcome run_phoneme_inventory_designer() {
    phoneme_inventory_designer::init();
    auto s = phoneme_inventory_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    phoneme_inventory_designer::shutdown();
    return testing::ok();
}

const int _reg_phoneme_inventory_designer = testing::register_test(
    "phoneme_inventory_designer",
    "804_phoneme_inventory_designer: stub status check",
    &run_phoneme_inventory_designer);

}
