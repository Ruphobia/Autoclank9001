#include "test_runner.hpp"
#include "../modules/1076_propeller_series_matcher/propeller_series_matcher.hpp"

namespace {

testing::TestOutcome run_propeller_series_matcher() {
    propeller_series_matcher::init();
    auto s = propeller_series_matcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    propeller_series_matcher::shutdown();
    return testing::ok();
}

const int _reg_propeller_series_matcher = testing::register_test(
    "propeller_series_matcher",
    "1076_propeller_series_matcher: stub status check",
    &run_propeller_series_matcher);

}
