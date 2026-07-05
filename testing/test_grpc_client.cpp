#include "test_runner.hpp"
#include "../modules/136_grpc_client/grpc_client.hpp"

namespace {

testing::TestOutcome run_grpc_client() {
    grpc_client::init();
    auto s = grpc_client::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    grpc_client::shutdown();
    return testing::ok();
}

const int _reg_grpc_client = testing::register_test(
    "grpc_client",
    "136_grpc_client: stub status check",
    &run_grpc_client);

}
