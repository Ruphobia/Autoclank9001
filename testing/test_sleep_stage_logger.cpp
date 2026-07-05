#include "test_runner.hpp"
#include "../modules/2150_sleep_stage_logger/sleep_stage_logger.hpp"

namespace {

testing::TestOutcome run_sleep_stage_logger() {
    sleep_stage_logger::init();
    auto s = sleep_stage_logger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sleep_stage_logger::shutdown();
    return testing::ok();
}

const int _reg_sleep_stage_logger = testing::register_test(
    "sleep_stage_logger",
    "2150_sleep_stage_logger: stub status check",
    &run_sleep_stage_logger);

}
