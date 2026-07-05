#include "test_runner.hpp"
#include "../modules/1701_usd_stage_editor/usd_stage_editor.hpp"

namespace {

testing::TestOutcome run_usd_stage_editor() {
    usd_stage_editor::init();
    auto s = usd_stage_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    usd_stage_editor::shutdown();
    return testing::ok();
}

const int _reg_usd_stage_editor = testing::register_test(
    "usd_stage_editor",
    "1701_usd_stage_editor: stub status check",
    &run_usd_stage_editor);

}
