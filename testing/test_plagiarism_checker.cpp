#include "test_runner.hpp"
#include "../modules/519_plagiarism_checker/plagiarism_checker.hpp"

namespace {

testing::TestOutcome run_plagiarism_checker() {
    plagiarism_checker::init();
    auto s = plagiarism_checker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    plagiarism_checker::shutdown();
    return testing::ok();
}

const int _reg_plagiarism_checker = testing::register_test(
    "plagiarism_checker",
    "519_plagiarism_checker: stub status check",
    &run_plagiarism_checker);

}
