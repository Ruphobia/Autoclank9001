#include "test_runner.hpp"
#include "../modules/133_http_client/http_client.hpp"

namespace {

testing::TestOutcome run_http_client() {
    http_client::init();
    auto s = http_client::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    http_client::shutdown();
    return testing::ok();
}

const int _reg_http_client = testing::register_test(
    "http_client",
    "133_http_client: stub status check",
    &run_http_client);

}
