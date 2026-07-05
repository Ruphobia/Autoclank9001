#include "test_runner.hpp"
#include "../modules/2042_species_season_calendar/species_season_calendar.hpp"

namespace {

testing::TestOutcome run_species_season_calendar() {
    species_season_calendar::init();
    auto s = species_season_calendar::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    species_season_calendar::shutdown();
    return testing::ok();
}

const int _reg_species_season_calendar = testing::register_test(
    "species_season_calendar",
    "2042_species_season_calendar: stub status check",
    &run_species_season_calendar);

}
