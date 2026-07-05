#include "test_runner.hpp"
#include "../modules/1946_character_sheet_forge/character_sheet_forge.hpp"

namespace {

testing::TestOutcome run_character_sheet_forge() {
    character_sheet_forge::init();
    auto s = character_sheet_forge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    character_sheet_forge::shutdown();
    return testing::ok();
}

const int _reg_character_sheet_forge = testing::register_test(
    "character_sheet_forge",
    "1946_character_sheet_forge: stub status check",
    &run_character_sheet_forge);

}
