#include "test_runner.hpp"
#include "../modules/516_lesson_plan_composer/lesson_plan_composer.hpp"

namespace {

testing::TestOutcome run_lesson_plan_composer() {
    lesson_plan_composer::init();
    auto s = lesson_plan_composer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lesson_plan_composer::shutdown();
    return testing::ok();
}

const int _reg_lesson_plan_composer = testing::register_test(
    "lesson_plan_composer",
    "516_lesson_plan_composer: stub status check",
    &run_lesson_plan_composer);

}
