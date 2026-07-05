// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/217_filesystem_and_storage_designer/filesystem_and_storage_designer.hpp"

namespace {

testing::TestOutcome run_filesystem_and_storage_designer() {
    filesystem_and_storage_designer::init();
    auto s = filesystem_and_storage_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    filesystem_and_storage_designer::shutdown();
    return testing::ok();
}

const int _reg_filesystem_and_storage_designer = testing::register_test(
    "filesystem_and_storage_designer",
    "217_filesystem_and_storage_designer: stub status check",
    &run_filesystem_and_storage_designer);

}
