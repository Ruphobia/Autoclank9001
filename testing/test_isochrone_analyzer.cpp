#include "test_runner.hpp"
#include "../modules/495_isochrone_analyzer/isochrone_analyzer.hpp"

namespace {

testing::TestOutcome run_isochrone_analyzer() {
    isochrone_analyzer::init();
    auto s = isochrone_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    isochrone_analyzer::shutdown();
    return testing::ok();
}

const int _reg_isochrone_analyzer = testing::register_test(
    "isochrone_analyzer",
    "495_isochrone_analyzer: stub status check",
    &run_isochrone_analyzer);

}
