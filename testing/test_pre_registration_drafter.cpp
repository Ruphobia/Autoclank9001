#include "test_runner.hpp"
#include "../modules/1431_pre_registration_drafter/pre_registration_drafter.hpp"

namespace {

testing::TestOutcome run_pre_registration_drafter() {
    pre_registration_drafter::init();
    auto s = pre_registration_drafter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pre_registration_drafter::shutdown();
    return testing::ok();
}

const int _reg_pre_registration_drafter = testing::register_test(
    "pre_registration_drafter",
    "1431_pre_registration_drafter: stub status check",
    &run_pre_registration_drafter);

}
