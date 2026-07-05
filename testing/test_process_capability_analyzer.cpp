#include "test_runner.hpp"
#include "../modules/462_process_capability_analyzer/process_capability_analyzer.hpp"

namespace {

testing::TestOutcome run_process_capability_analyzer() {
    process_capability_analyzer::init();
    auto s = process_capability_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    process_capability_analyzer::shutdown();
    return testing::ok();
}

const int _reg_process_capability_analyzer = testing::register_test(
    "process_capability_analyzer",
    "462_process_capability_analyzer: stub status check",
    &run_process_capability_analyzer);

}
