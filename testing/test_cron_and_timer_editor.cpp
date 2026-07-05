#include "test_runner.hpp"
#include "../modules/260_cron_and_timer_editor/cron_and_timer_editor.hpp"

namespace {

testing::TestOutcome run_cron_and_timer_editor() {
    cron_and_timer_editor::init();
    auto s = cron_and_timer_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cron_and_timer_editor::shutdown();
    return testing::ok();
}

const int _reg_cron_and_timer_editor = testing::register_test(
    "cron_and_timer_editor",
    "260_cron_and_timer_editor: stub status check",
    &run_cron_and_timer_editor);

}
