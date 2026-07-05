#include "test_runner.hpp"
#include "../modules/1399_drug_interaction_checker/drug_interaction_checker.hpp"

namespace {

testing::TestOutcome run_drug_interaction_checker() {
    drug_interaction_checker::init();
    auto s = drug_interaction_checker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    drug_interaction_checker::shutdown();
    return testing::ok();
}

const int _reg_drug_interaction_checker = testing::register_test(
    "drug_interaction_checker",
    "1399_drug_interaction_checker: stub status check",
    &run_drug_interaction_checker);

}
