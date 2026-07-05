// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1518_ancient_tools_and_weapons_typology/ancient_tools_and_weapons_typology.hpp"

namespace {

testing::TestOutcome run_ancient_tools_and_weapons_typology() {
    ancient_tools_and_weapons_typology::init();
    auto s = ancient_tools_and_weapons_typology::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ancient_tools_and_weapons_typology::shutdown();
    return testing::ok();
}

const int _reg_ancient_tools_and_weapons_typology = testing::register_test(
    "ancient_tools_and_weapons_typology",
    "1518_ancient_tools_and_weapons_typology: stub status check",
    &run_ancient_tools_and_weapons_typology);

}
