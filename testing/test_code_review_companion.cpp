#include "test_runner.hpp"
#include "../modules/104_code_review_companion/code_review_companion.hpp"

namespace {

testing::TestOutcome run_code_review_companion() {
    code_review_companion::init();
    auto s = code_review_companion::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    code_review_companion::shutdown();
    return testing::ok();
}

const int _reg_code_review_companion = testing::register_test(
    "code_review_companion",
    "104_code_review_companion: stub status check",
    &run_code_review_companion);

}
