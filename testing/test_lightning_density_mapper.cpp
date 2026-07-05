// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1462_lightning_density_mapper/lightning_density_mapper.hpp"

namespace {

testing::TestOutcome run_lightning_density_mapper() {
    lightning_density_mapper::init();
    auto s = lightning_density_mapper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lightning_density_mapper::shutdown();
    return testing::ok();
}

const int _reg_lightning_density_mapper = testing::register_test(
    "lightning_density_mapper",
    "1462_lightning_density_mapper: stub status check",
    &run_lightning_density_mapper);

}
