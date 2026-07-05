#include "test_runner.hpp"
#include "../modules/842_scope_logic_analyzer_and_bench_instrument_integration/scope_logic_analyzer_and_bench_instrument_integration.hpp"

namespace {

testing::TestOutcome run_scope_logic_analyzer_and_bench_instrument_integration() {
    scope_logic_analyzer_and_bench_instrument_integration::init();
    auto s = scope_logic_analyzer_and_bench_instrument_integration::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    scope_logic_analyzer_and_bench_instrument_integration::shutdown();
    return testing::ok();
}

const int _reg_scope_logic_analyzer_and_bench_instrument_integration = testing::register_test(
    "scope_logic_analyzer_and_bench_instrument_integration",
    "842_scope_logic_analyzer_and_bench_instrument_integration: stub status check",
    &run_scope_logic_analyzer_and_bench_instrument_integration);

}
