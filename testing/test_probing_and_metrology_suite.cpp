#include "test_runner.hpp"
#include "../modules/956_probing_and_metrology_suite/probing_and_metrology_suite.hpp"

namespace {

testing::TestOutcome run_probing_and_metrology_suite() {
    probing_and_metrology_suite::init();
    auto s = probing_and_metrology_suite::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    probing_and_metrology_suite::shutdown();
    return testing::ok();
}

const int _reg_probing_and_metrology_suite = testing::register_test(
    "probing_and_metrology_suite",
    "956_probing_and_metrology_suite: stub status check",
    &run_probing_and_metrology_suite);

}
