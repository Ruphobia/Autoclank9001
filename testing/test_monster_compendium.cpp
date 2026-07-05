#include "test_runner.hpp"
#include "../modules/1956_monster_compendium/monster_compendium.hpp"

namespace {

testing::TestOutcome run_monster_compendium() {
    monster_compendium::init();
    auto s = monster_compendium::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    monster_compendium::shutdown();
    return testing::ok();
}

const int _reg_monster_compendium = testing::register_test(
    "monster_compendium",
    "1956_monster_compendium: stub status check",
    &run_monster_compendium);

}
