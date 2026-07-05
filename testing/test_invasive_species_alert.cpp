#include "test_runner.hpp"
#include "../modules/1569_invasive_species_alert/invasive_species_alert.hpp"

namespace {

testing::TestOutcome run_invasive_species_alert() {
    invasive_species_alert::init();
    auto s = invasive_species_alert::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    invasive_species_alert::shutdown();
    return testing::ok();
}

const int _reg_invasive_species_alert = testing::register_test(
    "invasive_species_alert",
    "1569_invasive_species_alert: stub status check",
    &run_invasive_species_alert);

}
