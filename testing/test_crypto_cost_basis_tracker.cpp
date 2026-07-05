#include "test_runner.hpp"
#include "../modules/568_crypto_cost_basis_tracker/crypto_cost_basis_tracker.hpp"

namespace {

testing::TestOutcome run_crypto_cost_basis_tracker() {
    crypto_cost_basis_tracker::init();
    auto s = crypto_cost_basis_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    crypto_cost_basis_tracker::shutdown();
    return testing::ok();
}

const int _reg_crypto_cost_basis_tracker = testing::register_test(
    "crypto_cost_basis_tracker",
    "568_crypto_cost_basis_tracker: stub status check",
    &run_crypto_cost_basis_tracker);

}
