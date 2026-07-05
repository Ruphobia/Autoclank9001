#include "test_runner.hpp"
#include "../modules/2090_drill_and_play_diagrammer/drill_and_play_diagrammer.hpp"

namespace {

testing::TestOutcome run_drill_and_play_diagrammer() {
    drill_and_play_diagrammer::init();
    auto s = drill_and_play_diagrammer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    drill_and_play_diagrammer::shutdown();
    return testing::ok();
}

const int _reg_drill_and_play_diagrammer = testing::register_test(
    "drill_and_play_diagrammer",
    "2090_drill_and_play_diagrammer: stub status check",
    &run_drill_and_play_diagrammer);

}
