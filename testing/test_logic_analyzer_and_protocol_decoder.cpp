#include "test_runner.hpp"
#include "../modules/858_logic_analyzer_and_protocol_decoder/logic_analyzer_and_protocol_decoder.hpp"

namespace {

testing::TestOutcome run_logic_analyzer_and_protocol_decoder() {
    logic_analyzer_and_protocol_decoder::init();
    auto s = logic_analyzer_and_protocol_decoder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    logic_analyzer_and_protocol_decoder::shutdown();
    return testing::ok();
}

const int _reg_logic_analyzer_and_protocol_decoder = testing::register_test(
    "logic_analyzer_and_protocol_decoder",
    "858_logic_analyzer_and_protocol_decoder: stub status check",
    &run_logic_analyzer_and_protocol_decoder);

}
