#include "test_runner.hpp"
#include "../modules/2174_consensus_sleep_diary/consensus_sleep_diary.hpp"

namespace {

testing::TestOutcome run_consensus_sleep_diary() {
    consensus_sleep_diary::init();
    auto s = consensus_sleep_diary::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    consensus_sleep_diary::shutdown();
    return testing::ok();
}

const int _reg_consensus_sleep_diary = testing::register_test(
    "consensus_sleep_diary",
    "2174_consensus_sleep_diary: stub status check",
    &run_consensus_sleep_diary);

}
