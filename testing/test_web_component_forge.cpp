#include "test_runner.hpp"
#include "../modules/131_web_component_forge/web_component_forge.hpp"

namespace {

testing::TestOutcome run_web_component_forge() {
    web_component_forge::init();
    auto s = web_component_forge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    web_component_forge::shutdown();
    return testing::ok();
}

const int _reg_web_component_forge = testing::register_test(
    "web_component_forge",
    "131_web_component_forge: stub status check",
    &run_web_component_forge);

}
