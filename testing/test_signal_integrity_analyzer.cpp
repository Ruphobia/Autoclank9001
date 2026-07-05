#include "test_runner.hpp"
#include "../modules/852_signal_integrity_analyzer/signal_integrity_analyzer.hpp"

namespace {

testing::TestOutcome run_signal_integrity_analyzer() {
    signal_integrity_analyzer::init();
    auto s = signal_integrity_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    signal_integrity_analyzer::shutdown();
    return testing::ok();
}

const int _reg_signal_integrity_analyzer = testing::register_test(
    "signal_integrity_analyzer",
    "852_signal_integrity_analyzer: stub status check",
    &run_signal_integrity_analyzer);

}
