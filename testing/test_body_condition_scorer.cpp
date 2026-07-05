#include "test_runner.hpp"
#include "../modules/1505_body_condition_scorer/body_condition_scorer.hpp"

namespace {

testing::TestOutcome run_body_condition_scorer() {
    body_condition_scorer::init();
    auto s = body_condition_scorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    body_condition_scorer::shutdown();
    return testing::ok();
}

const int _reg_body_condition_scorer = testing::register_test(
    "body_condition_scorer",
    "1505_body_condition_scorer: stub status check",
    &run_body_condition_scorer);

}
