#include "test_runner.hpp"
#include "../modules/1745_mastering_workbench/mastering_workbench.hpp"

namespace {

testing::TestOutcome run_mastering_workbench() {
    mastering_workbench::init();
    auto s = mastering_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mastering_workbench::shutdown();
    return testing::ok();
}

const int _reg_mastering_workbench = testing::register_test(
    "mastering_workbench",
    "1745_mastering_workbench: stub status check",
    &run_mastering_workbench);

}
