#include "test_runner.hpp"
#include "../modules/206_symbol_and_call_graph_explorer/symbol_and_call_graph_explorer.hpp"

namespace {

testing::TestOutcome run_symbol_and_call_graph_explorer() {
    symbol_and_call_graph_explorer::init();
    auto s = symbol_and_call_graph_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    symbol_and_call_graph_explorer::shutdown();
    return testing::ok();
}

const int _reg_symbol_and_call_graph_explorer = testing::register_test(
    "symbol_and_call_graph_explorer",
    "206_symbol_and_call_graph_explorer: stub status check",
    &run_symbol_and_call_graph_explorer);

}
