#include "test_runner.hpp"
#include "../modules/608_address_verifier/address_verifier.hpp"

namespace {

testing::TestOutcome run_address_verifier() {
    address_verifier::init();
    auto s = address_verifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    address_verifier::shutdown();
    return testing::ok();
}

const int _reg_address_verifier = testing::register_test(
    "address_verifier",
    "608_address_verifier: stub status check",
    &run_address_verifier);

}
