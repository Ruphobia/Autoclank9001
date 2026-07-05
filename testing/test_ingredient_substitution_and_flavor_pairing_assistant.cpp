#include "test_runner.hpp"
#include "../modules/1817_ingredient_substitution_and_flavor_pairing_assistant/ingredient_substitution_and_flavor_pairing_assistant.hpp"

namespace {

testing::TestOutcome run_ingredient_substitution_and_flavor_pairing_assistant() {
    ingredient_substitution_and_flavor_pairing_assistant::init();
    auto s = ingredient_substitution_and_flavor_pairing_assistant::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ingredient_substitution_and_flavor_pairing_assistant::shutdown();
    return testing::ok();
}

const int _reg_ingredient_substitution_and_flavor_pairing_assistant = testing::register_test(
    "ingredient_substitution_and_flavor_pairing_assistant",
    "1817_ingredient_substitution_and_flavor_pairing_assistant: stub status check",
    &run_ingredient_substitution_and_flavor_pairing_assistant);

}
