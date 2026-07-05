#include "test_runner.hpp"
#include "../modules/1574_red_list_status_checker/red_list_status_checker.hpp"

namespace {

testing::TestOutcome run_red_list_status_checker() {
    red_list_status_checker::init();
    auto s = red_list_status_checker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    red_list_status_checker::shutdown();
    return testing::ok();
}

const int _reg_red_list_status_checker = testing::register_test(
    "red_list_status_checker",
    "1574_red_list_status_checker: stub status check",
    &run_red_list_status_checker);

}
