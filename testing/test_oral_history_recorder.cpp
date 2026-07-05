#include "test_runner.hpp"
#include "../modules/770_oral_history_recorder/oral_history_recorder.hpp"

namespace {

testing::TestOutcome run_oral_history_recorder() {
    oral_history_recorder::init();
    auto s = oral_history_recorder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    oral_history_recorder::shutdown();
    return testing::ok();
}

const int _reg_oral_history_recorder = testing::register_test(
    "oral_history_recorder",
    "770_oral_history_recorder: stub status check",
    &run_oral_history_recorder);

}
