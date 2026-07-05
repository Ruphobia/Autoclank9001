#include "test_runner.hpp"
#include "../modules/1128_tle_workbench/tle_workbench.hpp"

namespace {

testing::TestOutcome run_tle_workbench() {
    tle_workbench::init();
    auto s = tle_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tle_workbench::shutdown();
    return testing::ok();
}

const int _reg_tle_workbench = testing::register_test(
    "tle_workbench",
    "1128_tle_workbench: stub status check",
    &run_tle_workbench);

}
