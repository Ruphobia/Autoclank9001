#include "test_runner.hpp"
#include "../modules/167_localizable_strings_editor/localizable_strings_editor.hpp"

namespace {

testing::TestOutcome run_localizable_strings_editor() {
    localizable_strings_editor::init();
    auto s = localizable_strings_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    localizable_strings_editor::shutdown();
    return testing::ok();
}

const int _reg_localizable_strings_editor = testing::register_test(
    "localizable_strings_editor",
    "167_localizable_strings_editor: stub status check",
    &run_localizable_strings_editor);

}
