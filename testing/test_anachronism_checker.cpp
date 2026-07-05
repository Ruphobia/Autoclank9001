#include "test_runner.hpp"
#include "../modules/509_anachronism_checker/anachronism_checker.hpp"

namespace {

testing::TestOutcome run_anachronism_checker() {
    anachronism_checker::init();
    auto s = anachronism_checker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    anachronism_checker::shutdown();
    return testing::ok();
}

const int _reg_anachronism_checker = testing::register_test(
    "anachronism_checker",
    "509_anachronism_checker: stub status check",
    &run_anachronism_checker);

}
