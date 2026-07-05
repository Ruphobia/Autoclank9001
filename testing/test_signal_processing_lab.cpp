#include "test_runner.hpp"
#include "../modules/914_signal_processing_lab/signal_processing_lab.hpp"

namespace {

testing::TestOutcome run_signal_processing_lab() {
    signal_processing_lab::init();
    auto s = signal_processing_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    signal_processing_lab::shutdown();
    return testing::ok();
}

const int _reg_signal_processing_lab = testing::register_test(
    "signal_processing_lab",
    "914_signal_processing_lab: stub status check",
    &run_signal_processing_lab);

}
