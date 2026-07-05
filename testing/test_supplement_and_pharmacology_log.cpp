// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2167_supplement_and_pharmacology_log/supplement_and_pharmacology_log.hpp"

namespace {

testing::TestOutcome run_supplement_and_pharmacology_log() {
    supplement_and_pharmacology_log::init();
    auto s = supplement_and_pharmacology_log::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    supplement_and_pharmacology_log::shutdown();
    return testing::ok();
}

const int _reg_supplement_and_pharmacology_log = testing::register_test(
    "supplement_and_pharmacology_log",
    "2167_supplement_and_pharmacology_log: stub status check",
    &run_supplement_and_pharmacology_log);

}
