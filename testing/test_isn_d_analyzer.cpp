#include "test_runner.hpp"
#include "../modules/763_isn_d_analyzer/isn_d_analyzer.hpp"

namespace {

testing::TestOutcome run_isn_d_analyzer() {
    isn_d_analyzer::init();
    auto s = isn_d_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    isn_d_analyzer::shutdown();
    return testing::ok();
}

const int _reg_isn_d_analyzer = testing::register_test(
    "isn_d_analyzer",
    "763_isn_d_analyzer: stub status check",
    &run_isn_d_analyzer);

}
