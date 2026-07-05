// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/746_hymnody_and_chant_archive/hymnody_and_chant_archive.hpp"

namespace {

testing::TestOutcome run_hymnody_and_chant_archive() {
    hymnody_and_chant_archive::init();
    auto s = hymnody_and_chant_archive::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hymnody_and_chant_archive::shutdown();
    return testing::ok();
}

const int _reg_hymnody_and_chant_archive = testing::register_test(
    "hymnody_and_chant_archive",
    "746_hymnody_and_chant_archive: stub status check",
    &run_hymnody_and_chant_archive);

}
