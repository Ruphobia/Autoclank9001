// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1762_project_versioning_and_stem_archive/project_versioning_and_stem_archive.hpp"

namespace {

testing::TestOutcome run_project_versioning_and_stem_archive() {
    project_versioning_and_stem_archive::init();
    auto s = project_versioning_and_stem_archive::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    project_versioning_and_stem_archive::shutdown();
    return testing::ok();
}

const int _reg_project_versioning_and_stem_archive = testing::register_test(
    "project_versioning_and_stem_archive",
    "1762_project_versioning_and_stem_archive: stub status check",
    &run_project_versioning_and_stem_archive);

}
