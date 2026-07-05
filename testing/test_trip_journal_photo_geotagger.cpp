// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2070_trip_journal_photo_geotagger/trip_journal_photo_geotagger.hpp"

namespace {

testing::TestOutcome run_trip_journal_photo_geotagger() {
    trip_journal_photo_geotagger::init();
    auto s = trip_journal_photo_geotagger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    trip_journal_photo_geotagger::shutdown();
    return testing::ok();
}

const int _reg_trip_journal_photo_geotagger = testing::register_test(
    "trip_journal_photo_geotagger",
    "2070_trip_journal_photo_geotagger: stub status check",
    &run_trip_journal_photo_geotagger);

}
