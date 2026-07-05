#include "test_runner.hpp"
#include "../modules/1138_gravity_field_explorer/gravity_field_explorer.hpp"

namespace {

testing::TestOutcome run_gravity_field_explorer() {
    gravity_field_explorer::init();
    auto s = gravity_field_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gravity_field_explorer::shutdown();
    return testing::ok();
}

const int _reg_gravity_field_explorer = testing::register_test(
    "gravity_field_explorer",
    "1138_gravity_field_explorer: stub status check",
    &run_gravity_field_explorer);

}
