#include "test_runner.hpp"
#include "../modules/110_go_toolchain/go_toolchain.hpp"

namespace {

testing::TestOutcome run_go_toolchain() {
    go_toolchain::init();
    auto s = go_toolchain::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    go_toolchain::shutdown();
    return testing::ok();
}

const int _reg_go_toolchain = testing::register_test(
    "go_toolchain",
    "110_go_toolchain: stub status check",
    &run_go_toolchain);

}
