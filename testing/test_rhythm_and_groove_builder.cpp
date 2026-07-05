#include "test_runner.hpp"
#include "../modules/1715_rhythm_and_groove_builder/rhythm_and_groove_builder.hpp"

namespace {

testing::TestOutcome run_rhythm_and_groove_builder() {
    rhythm_and_groove_builder::init();
    auto s = rhythm_and_groove_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rhythm_and_groove_builder::shutdown();
    return testing::ok();
}

const int _reg_rhythm_and_groove_builder = testing::register_test(
    "rhythm_and_groove_builder",
    "1715_rhythm_and_groove_builder: stub status check",
    &run_rhythm_and_groove_builder);

}
