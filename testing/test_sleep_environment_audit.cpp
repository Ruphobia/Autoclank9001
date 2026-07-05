#include "test_runner.hpp"
#include "../modules/2168_sleep_environment_audit/sleep_environment_audit.hpp"

namespace {

testing::TestOutcome run_sleep_environment_audit() {
    sleep_environment_audit::init();
    auto s = sleep_environment_audit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sleep_environment_audit::shutdown();
    return testing::ok();
}

const int _reg_sleep_environment_audit = testing::register_test(
    "sleep_environment_audit",
    "2168_sleep_environment_audit: stub status check",
    &run_sleep_environment_audit);

}
