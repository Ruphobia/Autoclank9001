#include "test_runner.hpp"
#include "../modules/569_financial_statement_analyzer/financial_statement_analyzer.hpp"

namespace {

testing::TestOutcome run_financial_statement_analyzer() {
    financial_statement_analyzer::init();
    auto s = financial_statement_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    financial_statement_analyzer::shutdown();
    return testing::ok();
}

const int _reg_financial_statement_analyzer = testing::register_test(
    "financial_statement_analyzer",
    "569_financial_statement_analyzer: stub status check",
    &run_financial_statement_analyzer);

}
