#include "test_runner.hpp"
#include "../modules/1688_node_compositor/node_compositor.hpp"

namespace {

testing::TestOutcome run_node_compositor() {
    node_compositor::init();
    auto s = node_compositor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    node_compositor::shutdown();
    return testing::ok();
}

const int _reg_node_compositor = testing::register_test(
    "node_compositor",
    "1688_node_compositor: stub status check",
    &run_node_compositor);

}
