// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/967_filament_and_resin_library/filament_and_resin_library.hpp"

namespace {

testing::TestOutcome run_filament_and_resin_library() {
    filament_and_resin_library::init();
    auto s = filament_and_resin_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    filament_and_resin_library::shutdown();
    return testing::ok();
}

const int _reg_filament_and_resin_library = testing::register_test(
    "filament_and_resin_library",
    "967_filament_and_resin_library: stub status check",
    &run_filament_and_resin_library);

}
