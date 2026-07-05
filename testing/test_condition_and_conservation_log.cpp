#include "test_runner.hpp"
#include "../modules/1887_condition_and_conservation_log/condition_and_conservation_log.hpp"

namespace {

testing::TestOutcome run_condition_and_conservation_log() {
    condition_and_conservation_log::init();
    auto s = condition_and_conservation_log::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    condition_and_conservation_log::shutdown();
    return testing::ok();
}

const int _reg_condition_and_conservation_log = testing::register_test(
    "condition_and_conservation_log",
    "1887_condition_and_conservation_log: stub status check",
    &run_condition_and_conservation_log);

}
