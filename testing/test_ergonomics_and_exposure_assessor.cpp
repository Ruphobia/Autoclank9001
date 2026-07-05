// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/477_ergonomics_and_exposure_assessor/ergonomics_and_exposure_assessor.hpp"

namespace {

testing::TestOutcome run_ergonomics_and_exposure_assessor() {
    ergonomics_and_exposure_assessor::init();
    auto s = ergonomics_and_exposure_assessor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ergonomics_and_exposure_assessor::shutdown();
    return testing::ok();
}

const int _reg_ergonomics_and_exposure_assessor = testing::register_test(
    "ergonomics_and_exposure_assessor",
    "477_ergonomics_and_exposure_assessor: stub status check",
    &run_ergonomics_and_exposure_assessor);

}
