#include "test_runner.hpp"
#include "../modules/739_sacred_geography_atlas/sacred_geography_atlas.hpp"

namespace {

testing::TestOutcome run_sacred_geography_atlas() {
    sacred_geography_atlas::init();
    auto s = sacred_geography_atlas::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sacred_geography_atlas::shutdown();
    return testing::ok();
}

const int _reg_sacred_geography_atlas = testing::register_test(
    "sacred_geography_atlas",
    "739_sacred_geography_atlas: stub status check",
    &run_sacred_geography_atlas);

}
