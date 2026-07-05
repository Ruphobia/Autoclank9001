#include "test_runner.hpp"
#include "../modules/702_genealogy_and_gedcom_editor/genealogy_and_gedcom_editor.hpp"

namespace {

testing::TestOutcome run_genealogy_and_gedcom_editor() {
    genealogy_and_gedcom_editor::init();
    auto s = genealogy_and_gedcom_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    genealogy_and_gedcom_editor::shutdown();
    return testing::ok();
}

const int _reg_genealogy_and_gedcom_editor = testing::register_test(
    "genealogy_and_gedcom_editor",
    "702_genealogy_and_gedcom_editor: stub status check",
    &run_genealogy_and_gedcom_editor);

}
