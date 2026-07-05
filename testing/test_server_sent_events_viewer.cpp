#include "test_runner.hpp"
#include "../modules/150_server_sent_events_viewer/server_sent_events_viewer.hpp"

namespace {

testing::TestOutcome run_server_sent_events_viewer() {
    server_sent_events_viewer::init();
    auto s = server_sent_events_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    server_sent_events_viewer::shutdown();
    return testing::ok();
}

const int _reg_server_sent_events_viewer = testing::register_test(
    "server_sent_events_viewer",
    "150_server_sent_events_viewer: stub status check",
    &run_server_sent_events_viewer);

}
