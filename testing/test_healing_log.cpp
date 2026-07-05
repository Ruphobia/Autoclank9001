#include "test_runner.hpp"
#include "../modules/1903_healing_log/healing_log.hpp"

namespace {

testing::TestOutcome run_healing_log() {
    healing_log::init();
    auto s = healing_log::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    healing_log::shutdown();
    return testing::ok();
}

const int _reg_healing_log = testing::register_test(
    "healing_log",
    "1903_healing_log: stub status check",
    &run_healing_log);

}
