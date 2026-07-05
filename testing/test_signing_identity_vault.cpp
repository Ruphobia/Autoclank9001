#include "test_runner.hpp"
#include "../modules/163_signing_identity_vault/signing_identity_vault.hpp"

namespace {

testing::TestOutcome run_signing_identity_vault() {
    signing_identity_vault::init();
    auto s = signing_identity_vault::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    signing_identity_vault::shutdown();
    return testing::ok();
}

const int _reg_signing_identity_vault = testing::register_test(
    "signing_identity_vault",
    "163_signing_identity_vault: stub status check",
    &run_signing_identity_vault);

}
