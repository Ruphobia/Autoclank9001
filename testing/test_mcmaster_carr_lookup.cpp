#include "test_runner.hpp"
#include "../modules/841_mcmaster_carr_lookup/mcmaster_carr_lookup.hpp"

namespace {

testing::TestOutcome run_mcmaster_carr_lookup() {
    mcmaster_carr_lookup::init();
    auto s = mcmaster_carr_lookup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mcmaster_carr_lookup::shutdown();
    return testing::ok();
}

const int _reg_mcmaster_carr_lookup = testing::register_test(
    "mcmaster_carr_lookup",
    "841_mcmaster_carr_lookup: stub status check",
    &run_mcmaster_carr_lookup);

}
