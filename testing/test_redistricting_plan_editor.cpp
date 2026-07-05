#include "test_runner.hpp"
#include "../modules/1440_redistricting_plan_editor/redistricting_plan_editor.hpp"

namespace {

testing::TestOutcome run_redistricting_plan_editor() {
    redistricting_plan_editor::init();
    auto s = redistricting_plan_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    redistricting_plan_editor::shutdown();
    return testing::ok();
}

const int _reg_redistricting_plan_editor = testing::register_test(
    "redistricting_plan_editor",
    "1440_redistricting_plan_editor: stub status check",
    &run_redistricting_plan_editor);

}
