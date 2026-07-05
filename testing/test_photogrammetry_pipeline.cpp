#include "test_runner.hpp"
#include "../modules/1525_photogrammetry_pipeline/photogrammetry_pipeline.hpp"

namespace {

testing::TestOutcome run_photogrammetry_pipeline() {
    photogrammetry_pipeline::init();
    auto s = photogrammetry_pipeline::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    photogrammetry_pipeline::shutdown();
    return testing::ok();
}

const int _reg_photogrammetry_pipeline = testing::register_test(
    "photogrammetry_pipeline",
    "1525_photogrammetry_pipeline: stub status check",
    &run_photogrammetry_pipeline);

}
