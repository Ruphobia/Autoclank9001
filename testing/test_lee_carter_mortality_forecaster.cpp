// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/547_lee_carter_mortality_forecaster/lee_carter_mortality_forecaster.hpp"

namespace {

testing::TestOutcome run_lee_carter_mortality_forecaster() {
    lee_carter_mortality_forecaster::init();
    auto s = lee_carter_mortality_forecaster::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lee_carter_mortality_forecaster::shutdown();
    return testing::ok();
}

const int _reg_lee_carter_mortality_forecaster = testing::register_test(
    "lee_carter_mortality_forecaster",
    "547_lee_carter_mortality_forecaster: stub status check",
    &run_lee_carter_mortality_forecaster);

}
