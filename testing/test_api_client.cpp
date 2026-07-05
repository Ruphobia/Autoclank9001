#include "test_runner.hpp"
#include "../modules/098_api_client/api_client.hpp"

namespace {

testing::TestOutcome run_api_client() {
    api_client::init();
    auto s = api_client::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    api_client::shutdown();
    return testing::ok();
}

const int _reg_api_client = testing::register_test(
    "api_client",
    "098_api_client: stub status check",
    &run_api_client);

}
