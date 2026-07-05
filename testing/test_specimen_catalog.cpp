// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1519_specimen_catalog/specimen_catalog.hpp"

namespace {

testing::TestOutcome run_specimen_catalog() {
    specimen_catalog::init();
    auto s = specimen_catalog::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    specimen_catalog::shutdown();
    return testing::ok();
}

const int _reg_specimen_catalog = testing::register_test(
    "specimen_catalog",
    "1519_specimen_catalog: stub status check",
    &run_specimen_catalog);

}
