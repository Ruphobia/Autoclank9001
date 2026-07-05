#include "test_runner.hpp"
#include "../modules/1898_body_placement_mockup/body_placement_mockup.hpp"

namespace {

testing::TestOutcome run_body_placement_mockup() {
    body_placement_mockup::init();
    auto s = body_placement_mockup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    body_placement_mockup::shutdown();
    return testing::ok();
}

const int _reg_body_placement_mockup = testing::register_test(
    "body_placement_mockup",
    "1898_body_placement_mockup: stub status check",
    &run_body_placement_mockup);

}
