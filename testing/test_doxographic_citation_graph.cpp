// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/760_doxographic_citation_graph/doxographic_citation_graph.hpp"

namespace {

testing::TestOutcome run_doxographic_citation_graph() {
    doxographic_citation_graph::init();
    auto s = doxographic_citation_graph::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    doxographic_citation_graph::shutdown();
    return testing::ok();
}

const int _reg_doxographic_citation_graph = testing::register_test(
    "doxographic_citation_graph",
    "760_doxographic_citation_graph: stub status check",
    &run_doxographic_citation_graph);

}
