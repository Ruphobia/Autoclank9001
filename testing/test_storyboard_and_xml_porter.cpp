// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/174_storyboard_and_xml_porter/storyboard_and_xml_porter.hpp"

namespace {

testing::TestOutcome run_storyboard_and_xml_porter() {
    storyboard_and_xml_porter::init();
    auto s = storyboard_and_xml_porter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    storyboard_and_xml_porter::shutdown();
    return testing::ok();
}

const int _reg_storyboard_and_xml_porter = testing::register_test(
    "storyboard_and_xml_porter",
    "174_storyboard_and_xml_porter: stub status check",
    &run_storyboard_and_xml_porter);

}
