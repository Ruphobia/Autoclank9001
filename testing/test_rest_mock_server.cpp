#include "test_runner.hpp"
#include "../modules/137_rest_mock_server/rest_mock_server.hpp"

namespace {

testing::TestOutcome run_rest_mock_server() {
    rest_mock_server::init();
    auto s = rest_mock_server::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rest_mock_server::shutdown();
    return testing::ok();
}

const int _reg_rest_mock_server = testing::register_test(
    "rest_mock_server",
    "137_rest_mock_server: stub status check",
    &run_rest_mock_server);

}
