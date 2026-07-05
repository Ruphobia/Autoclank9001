#include "test_runner.hpp"
#include "../modules/093_static_analyzer/static_analyzer.hpp"

namespace {

testing::TestOutcome run_static_analyzer() {
    static_analyzer::init();
    auto s = static_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    static_analyzer::shutdown();
    return testing::ok();
}

const int _reg_static_analyzer = testing::register_test(
    "static_analyzer",
    "093_static_analyzer: stub status check",
    &run_static_analyzer);

}
