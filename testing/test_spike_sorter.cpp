#include "test_runner.hpp"
#include "../modules/1392_spike_sorter/spike_sorter.hpp"

namespace {

testing::TestOutcome run_spike_sorter() {
    spike_sorter::init();
    auto s = spike_sorter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    spike_sorter::shutdown();
    return testing::ok();
}

const int _reg_spike_sorter = testing::register_test(
    "spike_sorter",
    "1392_spike_sorter: stub status check",
    &run_spike_sorter);

}
