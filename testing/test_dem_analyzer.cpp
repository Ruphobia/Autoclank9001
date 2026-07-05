#include "test_runner.hpp"
#include "../modules/1580_dem_analyzer/dem_analyzer.hpp"

namespace {

testing::TestOutcome run_dem_analyzer() {
    dem_analyzer::init();
    auto s = dem_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dem_analyzer::shutdown();
    return testing::ok();
}

const int _reg_dem_analyzer = testing::register_test(
    "dem_analyzer",
    "1580_dem_analyzer: stub status check",
    &run_dem_analyzer);

}
