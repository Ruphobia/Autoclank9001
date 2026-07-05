#include "test_runner.hpp"
#include "../modules/1759_drum_machine_and_step_sequencer/drum_machine_and_step_sequencer.hpp"

namespace {

testing::TestOutcome run_drum_machine_and_step_sequencer() {
    drum_machine_and_step_sequencer::init();
    auto s = drum_machine_and_step_sequencer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    drum_machine_and_step_sequencer::shutdown();
    return testing::ok();
}

const int _reg_drum_machine_and_step_sequencer = testing::register_test(
    "drum_machine_and_step_sequencer",
    "1759_drum_machine_and_step_sequencer: stub status check",
    &run_drum_machine_and_step_sequencer);

}
