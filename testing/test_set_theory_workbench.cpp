#include "test_runner.hpp"
#include "../modules/1718_set_theory_workbench/set_theory_workbench.hpp"

namespace {

testing::TestOutcome run_set_theory_workbench() {
    set_theory_workbench::init();
    auto s = set_theory_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    set_theory_workbench::shutdown();
    return testing::ok();
}

const int _reg_set_theory_workbench = testing::register_test(
    "set_theory_workbench",
    "1718_set_theory_workbench: stub status check",
    &run_set_theory_workbench);

}
