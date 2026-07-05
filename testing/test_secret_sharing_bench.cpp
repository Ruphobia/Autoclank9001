#include "test_runner.hpp"
#include "../modules/381_secret_sharing_bench/secret_sharing_bench.hpp"

namespace {

testing::TestOutcome run_secret_sharing_bench() {
    secret_sharing_bench::init();
    auto s = secret_sharing_bench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    secret_sharing_bench::shutdown();
    return testing::ok();
}

const int _reg_secret_sharing_bench = testing::register_test(
    "secret_sharing_bench",
    "381_secret_sharing_bench: stub status check",
    &run_secret_sharing_bench);

}
