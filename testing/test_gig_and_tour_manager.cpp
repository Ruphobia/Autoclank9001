// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1943_gig_and_tour_manager/gig_and_tour_manager.hpp"

namespace {

testing::TestOutcome run_gig_and_tour_manager() {
    gig_and_tour_manager::init();
    auto s = gig_and_tour_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gig_and_tour_manager::shutdown();
    return testing::ok();
}

const int _reg_gig_and_tour_manager = testing::register_test(
    "gig_and_tour_manager",
    "1943_gig_and_tour_manager: stub status check",
    &run_gig_and_tour_manager);

}
