// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1132_porkchop_plot_generator/porkchop_plot_generator.hpp"

namespace {

testing::TestOutcome run_porkchop_plot_generator() {
    porkchop_plot_generator::init();
    auto s = porkchop_plot_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    porkchop_plot_generator::shutdown();
    return testing::ok();
}

const int _reg_porkchop_plot_generator = testing::register_test(
    "porkchop_plot_generator",
    "1132_porkchop_plot_generator: stub status check",
    &run_porkchop_plot_generator);

}
