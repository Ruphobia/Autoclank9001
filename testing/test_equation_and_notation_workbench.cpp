#include "test_runner.hpp"
#include "../modules/647_equation_and_notation_workbench/equation_and_notation_workbench.hpp"

namespace {

testing::TestOutcome run_equation_and_notation_workbench() {
    equation_and_notation_workbench::init();
    auto s = equation_and_notation_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    equation_and_notation_workbench::shutdown();
    return testing::ok();
}

const int _reg_equation_and_notation_workbench = testing::register_test(
    "equation_and_notation_workbench",
    "647_equation_and_notation_workbench: stub status check",
    &run_equation_and_notation_workbench);

}
