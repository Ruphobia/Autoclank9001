#include "test_runner.hpp"
#include "../modules/1957_spell_and_item_library/spell_and_item_library.hpp"

namespace {

testing::TestOutcome run_spell_and_item_library() {
    spell_and_item_library::init();
    auto s = spell_and_item_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    spell_and_item_library::shutdown();
    return testing::ok();
}

const int _reg_spell_and_item_library = testing::register_test(
    "spell_and_item_library",
    "1957_spell_and_item_library: stub status check",
    &run_spell_and_item_library);

}
