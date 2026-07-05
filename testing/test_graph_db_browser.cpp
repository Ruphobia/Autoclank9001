#include "test_runner.hpp"
#include "../modules/239_graph_db_browser/graph_db_browser.hpp"

namespace {

testing::TestOutcome run_graph_db_browser() {
    graph_db_browser::init();
    auto s = graph_db_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    graph_db_browser::shutdown();
    return testing::ok();
}

const int _reg_graph_db_browser = testing::register_test(
    "graph_db_browser",
    "239_graph_db_browser: stub status check",
    &run_graph_db_browser);

}
