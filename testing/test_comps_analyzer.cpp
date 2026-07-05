#include "test_runner.hpp"
#include "../modules/536_comps_analyzer/comps_analyzer.hpp"

namespace {

testing::TestOutcome run_comps_analyzer() {
    comps_analyzer::init();
    auto s = comps_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    comps_analyzer::shutdown();
    return testing::ok();
}

const int _reg_comps_analyzer = testing::register_test(
    "comps_analyzer",
    "536_comps_analyzer: stub status check",
    &run_comps_analyzer);

}
