// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2007_pollinator_phenology_log/pollinator_phenology_log.hpp"

namespace {

testing::TestOutcome run_pollinator_phenology_log() {
    pollinator_phenology_log::init();
    auto s = pollinator_phenology_log::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pollinator_phenology_log::shutdown();
    return testing::ok();
}

const int _reg_pollinator_phenology_log = testing::register_test(
    "pollinator_phenology_log",
    "2007_pollinator_phenology_log: stub status check",
    &run_pollinator_phenology_log);

}
