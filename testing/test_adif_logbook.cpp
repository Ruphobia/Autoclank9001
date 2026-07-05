// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1177_adif_logbook/adif_logbook.hpp"

namespace {

testing::TestOutcome run_adif_logbook() {
    adif_logbook::init();
    auto s = adif_logbook::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    adif_logbook::shutdown();
    return testing::ok();
}

const int _reg_adif_logbook = testing::register_test(
    "adif_logbook",
    "1177_adif_logbook: stub status check",
    &run_adif_logbook);

}
