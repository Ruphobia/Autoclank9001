// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/457_project_template_library/project_template_library.hpp"

namespace {

testing::TestOutcome run_project_template_library() {
    project_template_library::init();
    auto s = project_template_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    project_template_library::shutdown();
    return testing::ok();
}

const int _reg_project_template_library = testing::register_test(
    "project_template_library",
    "457_project_template_library: stub status check",
    &run_project_template_library);

}
