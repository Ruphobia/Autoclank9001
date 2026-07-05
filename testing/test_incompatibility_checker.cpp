#include "test_runner.hpp"
#include "../modules/1310_incompatibility_checker/incompatibility_checker.hpp"

namespace {

testing::TestOutcome run_incompatibility_checker() {
    incompatibility_checker::init();
    auto s = incompatibility_checker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    incompatibility_checker::shutdown();
    return testing::ok();
}

const int _reg_incompatibility_checker = testing::register_test(
    "incompatibility_checker",
    "1310_incompatibility_checker: stub status check",
    &run_incompatibility_checker);

}
