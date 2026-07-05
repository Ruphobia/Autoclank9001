#include "test_runner.hpp"
#include "../modules/283_config_diff_and_version_tracker/config_diff_and_version_tracker.hpp"

namespace {

testing::TestOutcome run_config_diff_and_version_tracker() {
    config_diff_and_version_tracker::init();
    auto s = config_diff_and_version_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    config_diff_and_version_tracker::shutdown();
    return testing::ok();
}

const int _reg_config_diff_and_version_tracker = testing::register_test(
    "config_diff_and_version_tracker",
    "283_config_diff_and_version_tracker: stub status check",
    &run_config_diff_and_version_tracker);

}
