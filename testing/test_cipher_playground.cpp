#include "test_runner.hpp"
#include "../modules/374_cipher_playground/cipher_playground.hpp"

namespace {

testing::TestOutcome run_cipher_playground() {
    cipher_playground::init();
    auto s = cipher_playground::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cipher_playground::shutdown();
    return testing::ok();
}

const int _reg_cipher_playground = testing::register_test(
    "cipher_playground",
    "374_cipher_playground: stub status check",
    &run_cipher_playground);

}
