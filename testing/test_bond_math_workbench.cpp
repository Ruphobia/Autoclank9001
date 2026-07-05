#include "test_runner.hpp"
#include "../modules/565_bond_math_workbench/bond_math_workbench.hpp"

namespace {

testing::TestOutcome run_bond_math_workbench() {
    bond_math_workbench::init();
    auto s = bond_math_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bond_math_workbench::shutdown();
    return testing::ok();
}

const int _reg_bond_math_workbench = testing::register_test(
    "bond_math_workbench",
    "565_bond_math_workbench: stub status check",
    &run_bond_math_workbench);

}
