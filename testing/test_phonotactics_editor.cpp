#include "test_runner.hpp"
#include "../modules/805_phonotactics_editor/phonotactics_editor.hpp"

namespace {

testing::TestOutcome run_phonotactics_editor() {
    phonotactics_editor::init();
    auto s = phonotactics_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    phonotactics_editor::shutdown();
    return testing::ok();
}

const int _reg_phonotactics_editor = testing::register_test(
    "phonotactics_editor",
    "805_phonotactics_editor: stub status check",
    &run_phonotactics_editor);

}
