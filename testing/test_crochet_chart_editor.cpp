#include "test_runner.hpp"
#include "../modules/1772_crochet_chart_editor/crochet_chart_editor.hpp"

namespace {

testing::TestOutcome run_crochet_chart_editor() {
    crochet_chart_editor::init();
    auto s = crochet_chart_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    crochet_chart_editor::shutdown();
    return testing::ok();
}

const int _reg_crochet_chart_editor = testing::register_test(
    "crochet_chart_editor",
    "1772_crochet_chart_editor: stub status check",
    &run_crochet_chart_editor);

}
