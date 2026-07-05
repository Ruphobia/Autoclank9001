// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/527_socratic_tutor/socratic_tutor.hpp"

namespace {

testing::TestOutcome run_socratic_tutor() {
    socratic_tutor::init();
    auto s = socratic_tutor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    socratic_tutor::shutdown();
    return testing::ok();
}

const int _reg_socratic_tutor = testing::register_test(
    "socratic_tutor",
    "527_socratic_tutor: stub status check",
    &run_socratic_tutor);

}
