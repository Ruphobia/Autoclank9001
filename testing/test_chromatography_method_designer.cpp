// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1317_chromatography_method_designer/chromatography_method_designer.hpp"

namespace {

testing::TestOutcome run_chromatography_method_designer() {
    chromatography_method_designer::init();
    auto s = chromatography_method_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    chromatography_method_designer::shutdown();
    return testing::ok();
}

const int _reg_chromatography_method_designer = testing::register_test(
    "chromatography_method_designer",
    "1317_chromatography_method_designer: stub status check",
    &run_chromatography_method_designer);

}
