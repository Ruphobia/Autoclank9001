// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1769_embroidery_digitizer/embroidery_digitizer.hpp"

namespace {

testing::TestOutcome run_embroidery_digitizer() {
    embroidery_digitizer::init();
    auto s = embroidery_digitizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    embroidery_digitizer::shutdown();
    return testing::ok();
}

const int _reg_embroidery_digitizer = testing::register_test(
    "embroidery_digitizer",
    "1769_embroidery_digitizer: stub status check",
    &run_embroidery_digitizer);

}
