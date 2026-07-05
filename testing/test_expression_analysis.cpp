#include "test_runner.hpp"
#include "../modules/1334_expression_analysis/expression_analysis.hpp"

namespace {

testing::TestOutcome run_expression_analysis() {
    expression_analysis::init();
    auto s = expression_analysis::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    expression_analysis::shutdown();
    return testing::ok();
}

const int _reg_expression_analysis = testing::register_test(
    "expression_analysis",
    "1334_expression_analysis: stub status check",
    &run_expression_analysis);

}
