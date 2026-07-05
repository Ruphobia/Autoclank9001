#include "test_runner.hpp"
#include "../modules/1047_code_checker/code_checker.hpp"

namespace {

testing::TestOutcome run_code_checker() {
    code_checker::init();
    auto s = code_checker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    code_checker::shutdown();
    return testing::ok();
}

const int _reg_code_checker = testing::register_test(
    "code_checker",
    "1047_code_checker: stub status check",
    &run_code_checker);

}
