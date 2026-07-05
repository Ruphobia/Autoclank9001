#include "test_runner.hpp"
#include "../modules/2043_regulation_lookup/regulation_lookup.hpp"

namespace {

testing::TestOutcome run_regulation_lookup() {
    regulation_lookup::init();
    auto s = regulation_lookup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    regulation_lookup::shutdown();
    return testing::ok();
}

const int _reg_regulation_lookup = testing::register_test(
    "regulation_lookup",
    "2043_regulation_lookup: stub status check",
    &run_regulation_lookup);

}
