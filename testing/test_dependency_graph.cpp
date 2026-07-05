#include "test_runner.hpp"
#include "../modules/445_dependency_graph/dependency_graph.hpp"

namespace {

testing::TestOutcome run_dependency_graph() {
    dependency_graph::init();
    auto s = dependency_graph::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dependency_graph::shutdown();
    return testing::ok();
}

const int _reg_dependency_graph = testing::register_test(
    "dependency_graph",
    "445_dependency_graph: stub status check",
    &run_dependency_graph);

}
