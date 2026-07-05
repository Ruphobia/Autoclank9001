#include "test_runner.hpp"
#include "../modules/278_secrets_vault/secrets_vault.hpp"

namespace {

testing::TestOutcome run_secrets_vault() {
    secrets_vault::init();
    auto s = secrets_vault::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    secrets_vault::shutdown();
    return testing::ok();
}

const int _reg_secrets_vault = testing::register_test(
    "secrets_vault",
    "278_secrets_vault: stub status check",
    &run_secrets_vault);

}
