#include "test_runner.hpp"
#include "../modules/1748_signal_generator/signal_generator.hpp"

namespace {

testing::TestOutcome run_signal_generator() {
    signal_generator::init();
    auto s = signal_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    signal_generator::shutdown();
    return testing::ok();
}

const int _reg_signal_generator = testing::register_test(
    "signal_generator",
    "1748_signal_generator: stub status check",
    &run_signal_generator);

}
