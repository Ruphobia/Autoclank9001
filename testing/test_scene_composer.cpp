#include "test_runner.hpp"
#include "../modules/310_scene_composer/scene_composer.hpp"

namespace {

testing::TestOutcome run_scene_composer() {
    scene_composer::init();
    auto s = scene_composer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    scene_composer::shutdown();
    return testing::ok();
}

const int _reg_scene_composer = testing::register_test(
    "scene_composer",
    "310_scene_composer: stub status check",
    &run_scene_composer);

}
