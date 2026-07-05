#include "test_runner.hpp"
#include "../modules/1711_reharmonization_assistant/reharmonization_assistant.hpp"

namespace {

testing::TestOutcome run_reharmonization_assistant() {
    reharmonization_assistant::init();
    auto s = reharmonization_assistant::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reharmonization_assistant::shutdown();
    return testing::ok();
}

const int _reg_reharmonization_assistant = testing::register_test(
    "reharmonization_assistant",
    "1711_reharmonization_assistant: stub status check",
    &run_reharmonization_assistant);

}
