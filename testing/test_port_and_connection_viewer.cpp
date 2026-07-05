#include "test_runner.hpp"
#include "../modules/271_port_and_connection_viewer/port_and_connection_viewer.hpp"

namespace {

testing::TestOutcome run_port_and_connection_viewer() {
    port_and_connection_viewer::init();
    auto s = port_and_connection_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    port_and_connection_viewer::shutdown();
    return testing::ok();
}

const int _reg_port_and_connection_viewer = testing::register_test(
    "port_and_connection_viewer",
    "271_port_and_connection_viewer: stub status check",
    &run_port_and_connection_viewer);

}
