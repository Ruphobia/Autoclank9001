// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/455_changelog_and_release_notes_generator/changelog_and_release_notes_generator.hpp"

namespace {

testing::TestOutcome run_changelog_and_release_notes_generator() {
    changelog_and_release_notes_generator::init();
    auto s = changelog_and_release_notes_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    changelog_and_release_notes_generator::shutdown();
    return testing::ok();
}

const int _reg_changelog_and_release_notes_generator = testing::register_test(
    "changelog_and_release_notes_generator",
    "455_changelog_and_release_notes_generator: stub status check",
    &run_changelog_and_release_notes_generator);

}
