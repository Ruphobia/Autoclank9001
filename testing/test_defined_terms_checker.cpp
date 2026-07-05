#include "test_runner.hpp"
#include "../modules/581_defined_terms_checker/defined_terms_checker.hpp"

namespace {

testing::TestOutcome run_defined_terms_checker() {
    defined_terms_checker::init();
    auto s = defined_terms_checker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    defined_terms_checker::shutdown();
    return testing::ok();
}

const int _reg_defined_terms_checker = testing::register_test(
    "defined_terms_checker",
    "581_defined_terms_checker: stub status check",
    &run_defined_terms_checker);

}
