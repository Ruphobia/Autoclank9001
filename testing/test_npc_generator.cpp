#include "test_runner.hpp"
#include "../modules/1958_npc_generator/npc_generator.hpp"

namespace {

testing::TestOutcome run_npc_generator() {
    npc_generator::init();
    auto s = npc_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    npc_generator::shutdown();
    return testing::ok();
}

const int _reg_npc_generator = testing::register_test(
    "npc_generator",
    "1958_npc_generator: stub status check",
    &run_npc_generator);

}
