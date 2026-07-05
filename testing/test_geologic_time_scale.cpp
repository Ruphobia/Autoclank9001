#include "test_runner.hpp"
#include "../modules/1584_geologic_time_scale/geologic_time_scale.hpp"

namespace {

testing::TestOutcome run_geologic_time_scale() {
    geologic_time_scale::init();
    auto s = geologic_time_scale::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    geologic_time_scale::shutdown();
    return testing::ok();
}

const int _reg_geologic_time_scale = testing::register_test(
    "geologic_time_scale",
    "1584_geologic_time_scale: stub status check",
    &run_geologic_time_scale);

}
