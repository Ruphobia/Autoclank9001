#include "test_runner.hpp"
#include "../modules/505_quote_vault/quote_vault.hpp"

namespace {

testing::TestOutcome run_quote_vault() {
    quote_vault::init();
    auto s = quote_vault::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    quote_vault::shutdown();
    return testing::ok();
}

const int _reg_quote_vault = testing::register_test(
    "quote_vault",
    "505_quote_vault: stub status check",
    &run_quote_vault);

}
