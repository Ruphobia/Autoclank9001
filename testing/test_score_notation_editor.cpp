#include "test_runner.hpp"
#include "../modules/1706_score_notation_editor/score_notation_editor.hpp"

namespace {

testing::TestOutcome run_score_notation_editor() {
    score_notation_editor::init();
    auto s = score_notation_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    score_notation_editor::shutdown();
    return testing::ok();
}

const int _reg_score_notation_editor = testing::register_test(
    "score_notation_editor",
    "1706_score_notation_editor: stub status check",
    &run_score_notation_editor);

}
