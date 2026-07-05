#include "test_runner.hpp"
#include "../modules/387_contract_analyzer/contract_analyzer.hpp"

namespace {

testing::TestOutcome run_contract_analyzer() {
    contract_analyzer::init();
    auto s = contract_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    contract_analyzer::shutdown();
    return testing::ok();
}

const int _reg_contract_analyzer = testing::register_test(
    "contract_analyzer",
    "387_contract_analyzer: stub status check",
    &run_contract_analyzer);

}
