#include "test_runner.hpp"
#include "../modules/1973_tcg_deck_builder/tcg_deck_builder.hpp"

namespace {

testing::TestOutcome run_tcg_deck_builder() {
    tcg_deck_builder::init();
    auto s = tcg_deck_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tcg_deck_builder::shutdown();
    return testing::ok();
}

const int _reg_tcg_deck_builder = testing::register_test(
    "tcg_deck_builder",
    "1973_tcg_deck_builder: stub status check",
    &run_tcg_deck_builder);

}
