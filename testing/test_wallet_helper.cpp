#include "test_runner.hpp"
#include "../modules/392_wallet_helper/wallet_helper.hpp"

namespace {

testing::TestOutcome run_wallet_helper() {
    wallet_helper::init();
    auto s = wallet_helper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wallet_helper::shutdown();
    return testing::ok();
}

const int _reg_wallet_helper = testing::register_test(
    "wallet_helper",
    "392_wallet_helper: stub status check",
    &run_wallet_helper);

}
