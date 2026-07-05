// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/629_library_catalog/library_catalog.hpp"

namespace {

testing::TestOutcome run_library_catalog() {
    library_catalog::init();
    auto s = library_catalog::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    library_catalog::shutdown();
    return testing::ok();
}

const int _reg_library_catalog = testing::register_test(
    "library_catalog",
    "629_library_catalog: stub status check",
    &run_library_catalog);

}
