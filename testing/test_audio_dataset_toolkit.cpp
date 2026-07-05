#include "test_runner.hpp"
#include "../modules/683_audio_dataset_toolkit/audio_dataset_toolkit.hpp"

namespace {

testing::TestOutcome run_audio_dataset_toolkit() {
    audio_dataset_toolkit::init();
    auto s = audio_dataset_toolkit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    audio_dataset_toolkit::shutdown();
    return testing::ok();
}

const int _reg_audio_dataset_toolkit = testing::register_test(
    "audio_dataset_toolkit",
    "683_audio_dataset_toolkit: stub status check",
    &run_audio_dataset_toolkit);

}
