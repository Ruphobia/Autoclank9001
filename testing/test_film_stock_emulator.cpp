// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1678_film_stock_emulator/film_stock_emulator.hpp"

namespace {

testing::TestOutcome run_film_stock_emulator() {
    film_stock_emulator::init();
    auto s = film_stock_emulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    film_stock_emulator::shutdown();
    return testing::ok();
}

const int _reg_film_stock_emulator = testing::register_test(
    "film_stock_emulator",
    "1678_film_stock_emulator: stub status check",
    &run_film_stock_emulator);

}
