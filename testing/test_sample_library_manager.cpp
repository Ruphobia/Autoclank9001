// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1738_sample_library_manager/sample_library_manager.hpp"

namespace {

testing::TestOutcome run_sample_library_manager() {
    sample_library_manager::init();
    auto s = sample_library_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sample_library_manager::shutdown();
    return testing::ok();
}

const int _reg_sample_library_manager = testing::register_test(
    "sample_library_manager",
    "1738_sample_library_manager: stub status check",
    &run_sample_library_manager);

}
