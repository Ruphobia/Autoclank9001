#include "test_runner.hpp"
#include "../modules/236_redis_client/redis_client.hpp"

namespace {

testing::TestOutcome run_redis_client() {
    redis_client::init();
    auto s = redis_client::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    redis_client::shutdown();
    return testing::ok();
}

const int _reg_redis_client = testing::register_test(
    "redis_client",
    "236_redis_client: stub status check",
    &run_redis_client);

}
