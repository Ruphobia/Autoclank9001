#include "test_runner.hpp"
#include "../modules/1704_chord_and_scale_explorer/chord_and_scale_explorer.hpp"

namespace {

testing::TestOutcome run_chord_and_scale_explorer() {
    chord_and_scale_explorer::init();
    auto s = chord_and_scale_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    chord_and_scale_explorer::shutdown();
    return testing::ok();
}

const int _reg_chord_and_scale_explorer = testing::register_test(
    "chord_and_scale_explorer",
    "1704_chord_and_scale_explorer: stub status check",
    &run_chord_and_scale_explorer);

}
