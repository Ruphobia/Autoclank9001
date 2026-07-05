// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2065_paraglider_site_database/paraglider_site_database.hpp"

namespace {

testing::TestOutcome run_paraglider_site_database() {
    paraglider_site_database::init();
    auto s = paraglider_site_database::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    paraglider_site_database::shutdown();
    return testing::ok();
}

const int _reg_paraglider_site_database = testing::register_test(
    "paraglider_site_database",
    "2065_paraglider_site_database: stub status check",
    &run_paraglider_site_database);

}
