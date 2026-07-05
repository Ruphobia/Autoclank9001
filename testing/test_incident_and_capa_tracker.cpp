// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/470_incident_and_capa_tracker/incident_and_capa_tracker.hpp"

namespace {

testing::TestOutcome run_incident_and_capa_tracker() {
    incident_and_capa_tracker::init();
    auto s = incident_and_capa_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    incident_and_capa_tracker::shutdown();
    return testing::ok();
}

const int _reg_incident_and_capa_tracker = testing::register_test(
    "incident_and_capa_tracker",
    "470_incident_and_capa_tracker: stub status check",
    &run_incident_and_capa_tracker);

}
