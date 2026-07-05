#include "test_runner.hpp"
#include "../modules/551_crypto_trading_and_wallet_tools/crypto_trading_and_wallet_tools.hpp"

namespace {

testing::TestOutcome run_crypto_trading_and_wallet_tools() {
    crypto_trading_and_wallet_tools::init();
    auto s = crypto_trading_and_wallet_tools::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    crypto_trading_and_wallet_tools::shutdown();
    return testing::ok();
}

const int _reg_crypto_trading_and_wallet_tools = testing::register_test(
    "crypto_trading_and_wallet_tools",
    "551_crypto_trading_and_wallet_tools: stub status check",
    &run_crypto_trading_and_wallet_tools);

}
