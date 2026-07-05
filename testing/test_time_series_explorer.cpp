#include "test_runner.hpp"
#include "../modules/238_time_series_explorer/time_series_explorer.hpp"

namespace {

testing::TestOutcome run_time_series_explorer() {
    time_series_explorer::init();
    auto s = time_series_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    time_series_explorer::shutdown();
    return testing::ok();
}

const int _reg_time_series_explorer = testing::register_test(
    "time_series_explorer",
    "238_time_series_explorer: stub status check",
    &run_time_series_explorer);

}
