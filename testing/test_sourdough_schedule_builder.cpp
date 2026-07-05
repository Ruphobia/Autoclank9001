#include "test_runner.hpp"
#include "../modules/1795_sourdough_schedule_builder/sourdough_schedule_builder.hpp"

namespace {

testing::TestOutcome run_sourdough_schedule_builder() {
    sourdough_schedule_builder::init();
    auto s = sourdough_schedule_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sourdough_schedule_builder::shutdown();
    return testing::ok();
}

const int _reg_sourdough_schedule_builder = testing::register_test(
    "sourdough_schedule_builder",
    "1795_sourdough_schedule_builder: stub status check",
    &run_sourdough_schedule_builder);

}
