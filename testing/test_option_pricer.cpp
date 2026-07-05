#include "test_runner.hpp"
#include "../modules/564_option_pricer/option_pricer.hpp"

namespace {

testing::TestOutcome run_option_pricer() {
    option_pricer::init();
    auto s = option_pricer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    option_pricer::shutdown();
    return testing::ok();
}

const int _reg_option_pricer = testing::register_test(
    "option_pricer",
    "564_option_pricer: stub status check",
    &run_option_pricer);

}
