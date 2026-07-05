// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1561_reef_transect_logger/reef_transect_logger.hpp"

namespace {

testing::TestOutcome run_reef_transect_logger() {
    reef_transect_logger::init();
    auto s = reef_transect_logger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reef_transect_logger::shutdown();
    return testing::ok();
}

const int _reg_reef_transect_logger = testing::register_test(
    "reef_transect_logger",
    "1561_reef_transect_logger: stub status check",
    &run_reef_transect_logger);

}
