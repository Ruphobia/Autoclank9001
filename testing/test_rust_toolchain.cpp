#include "test_runner.hpp"
#include "../modules/109_rust_toolchain/rust_toolchain.hpp"

namespace {

testing::TestOutcome run_rust_toolchain() {
    rust_toolchain::init();
    auto s = rust_toolchain::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rust_toolchain::shutdown();
    return testing::ok();
}

const int _reg_rust_toolchain = testing::register_test(
    "rust_toolchain",
    "109_rust_toolchain: stub status check",
    &run_rust_toolchain);

}
