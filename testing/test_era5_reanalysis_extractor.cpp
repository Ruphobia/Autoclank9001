#include "test_runner.hpp"
#include "../modules/1467_era5_reanalysis_extractor/era5_reanalysis_extractor.hpp"

namespace {

testing::TestOutcome run_era5_reanalysis_extractor() {
    era5_reanalysis_extractor::init();
    auto s = era5_reanalysis_extractor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    era5_reanalysis_extractor::shutdown();
    return testing::ok();
}

const int _reg_era5_reanalysis_extractor = testing::register_test(
    "era5_reanalysis_extractor",
    "1467_era5_reanalysis_extractor: stub status check",
    &run_era5_reanalysis_extractor);

}
