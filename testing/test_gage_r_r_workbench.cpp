#include "test_runner.hpp"
#include "../modules/463_gage_r_r_workbench/gage_r_r_workbench.hpp"

namespace {

testing::TestOutcome run_gage_r_r_workbench() {
    gage_r_r_workbench::init();
    auto s = gage_r_r_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gage_r_r_workbench::shutdown();
    return testing::ok();
}

const int _reg_gage_r_r_workbench = testing::register_test(
    "gage_r_r_workbench",
    "463_gage_r_r_workbench: stub status check",
    &run_gage_r_r_workbench);

}
