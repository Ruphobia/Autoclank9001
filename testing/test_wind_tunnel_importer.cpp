// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1165_wind_tunnel_importer/wind_tunnel_importer.hpp"

namespace {

testing::TestOutcome run_wind_tunnel_importer() {
    wind_tunnel_importer::init();
    auto s = wind_tunnel_importer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wind_tunnel_importer::shutdown();
    return testing::ok();
}

const int _reg_wind_tunnel_importer = testing::register_test(
    "wind_tunnel_importer",
    "1165_wind_tunnel_importer: stub status check",
    &run_wind_tunnel_importer);

}
