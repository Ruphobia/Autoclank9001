#include "test_runner.hpp"
#include "../modules/675_eval_harness/eval_harness.hpp"

namespace {

testing::TestOutcome run_eval_harness() {
    eval_harness::init();
    auto s = eval_harness::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    eval_harness::shutdown();
    return testing::ok();
}

const int _reg_eval_harness = testing::register_test(
    "eval_harness",
    "675_eval_harness: stub status check",
    &run_eval_harness);

}
