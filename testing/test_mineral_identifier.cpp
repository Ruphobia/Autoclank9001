#include "test_runner.hpp"
#include "../modules/1583_mineral_identifier/mineral_identifier.hpp"

namespace {

testing::TestOutcome run_mineral_identifier() {
    mineral_identifier::init();
    auto s = mineral_identifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mineral_identifier::shutdown();
    return testing::ok();
}

const int _reg_mineral_identifier = testing::register_test(
    "mineral_identifier",
    "1583_mineral_identifier: stub status check",
    &run_mineral_identifier);

}
