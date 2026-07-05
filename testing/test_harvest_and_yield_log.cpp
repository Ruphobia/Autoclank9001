#include "test_runner.hpp"
#include "../modules/1619_harvest_and_yield_log/harvest_and_yield_log.hpp"

namespace {

testing::TestOutcome run_harvest_and_yield_log() {
    harvest_and_yield_log::init();
    auto s = harvest_and_yield_log::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    harvest_and_yield_log::shutdown();
    return testing::ok();
}

const int _reg_harvest_and_yield_log = testing::register_test(
    "harvest_and_yield_log",
    "1619_harvest_and_yield_log: stub status check",
    &run_harvest_and_yield_log);

}
