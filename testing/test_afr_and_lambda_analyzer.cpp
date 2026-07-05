#include "test_runner.hpp"
#include "../modules/1207_afr_and_lambda_analyzer/afr_and_lambda_analyzer.hpp"

namespace {

testing::TestOutcome run_afr_and_lambda_analyzer() {
    afr_and_lambda_analyzer::init();
    auto s = afr_and_lambda_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    afr_and_lambda_analyzer::shutdown();
    return testing::ok();
}

const int _reg_afr_and_lambda_analyzer = testing::register_test(
    "afr_and_lambda_analyzer",
    "1207_afr_and_lambda_analyzer: stub status check",
    &run_afr_and_lambda_analyzer);

}
