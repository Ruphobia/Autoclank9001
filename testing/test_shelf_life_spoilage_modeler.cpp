// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1802_shelf_life_spoilage_modeler/shelf_life_spoilage_modeler.hpp"

namespace {

testing::TestOutcome run_shelf_life_spoilage_modeler() {
    shelf_life_spoilage_modeler::init();
    auto s = shelf_life_spoilage_modeler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    shelf_life_spoilage_modeler::shutdown();
    return testing::ok();
}

const int _reg_shelf_life_spoilage_modeler = testing::register_test(
    "shelf_life_spoilage_modeler",
    "1802_shelf_life_spoilage_modeler: stub status check",
    &run_shelf_life_spoilage_modeler);

}
