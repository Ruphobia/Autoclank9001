// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/233_import_export_hub/import_export_hub.hpp"

namespace {

testing::TestOutcome run_import_export_hub() {
    import_export_hub::init();
    auto s = import_export_hub::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    import_export_hub::shutdown();
    return testing::ok();
}

const int _reg_import_export_hub = testing::register_test(
    "import_export_hub",
    "233_import_export_hub: stub status check",
    &run_import_export_hub);

}
