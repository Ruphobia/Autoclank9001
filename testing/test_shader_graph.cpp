#include "test_runner.hpp"
#include "../modules/1698_shader_graph/shader_graph.hpp"

namespace {

testing::TestOutcome run_shader_graph() {
    shader_graph::init();
    auto s = shader_graph::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    shader_graph::shutdown();
    return testing::ok();
}

const int _reg_shader_graph = testing::register_test(
    "shader_graph",
    "1698_shader_graph: stub status check",
    &run_shader_graph);

}
