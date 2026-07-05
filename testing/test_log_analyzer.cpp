#include "test_runner.hpp"
#include "../modules/355_log_analyzer/log_analyzer.hpp"

namespace {

testing::TestOutcome run_log_analyzer() {
    log_analyzer::init();
    auto s = log_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    log_analyzer::shutdown();
    return testing::ok();
}

const int _reg_log_analyzer = testing::register_test(
    "log_analyzer",
    "355_log_analyzer: stub status check",
    &run_log_analyzer);

}
