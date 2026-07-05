#include "test_runner.hpp"
#include "../modules/1867_value_and_notan_studio/value_and_notan_studio.hpp"

namespace {

testing::TestOutcome run_value_and_notan_studio() {
    value_and_notan_studio::init();
    auto s = value_and_notan_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    value_and_notan_studio::shutdown();
    return testing::ok();
}

const int _reg_value_and_notan_studio = testing::register_test(
    "value_and_notan_studio",
    "1867_value_and_notan_studio: stub status check",
    &run_value_and_notan_studio);

}
