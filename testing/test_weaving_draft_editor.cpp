#include "test_runner.hpp"
#include "../modules/1773_weaving_draft_editor/weaving_draft_editor.hpp"

namespace {

testing::TestOutcome run_weaving_draft_editor() {
    weaving_draft_editor::init();
    auto s = weaving_draft_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    weaving_draft_editor::shutdown();
    return testing::ok();
}

const int _reg_weaving_draft_editor = testing::register_test(
    "weaving_draft_editor",
    "1773_weaving_draft_editor: stub status check",
    &run_weaving_draft_editor);

}
