#include "test_runner.hpp"
#include "../modules/081_citation_graph_explorer/citation_graph_explorer.hpp"

namespace {

testing::TestOutcome run_citation_graph_explorer() {
    citation_graph_explorer::init();
    auto s = citation_graph_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    citation_graph_explorer::shutdown();
    return testing::ok();
}

const int _reg_citation_graph_explorer = testing::register_test(
    "citation_graph_explorer",
    "081_citation_graph_explorer: stub status check",
    &run_citation_graph_explorer);

}
