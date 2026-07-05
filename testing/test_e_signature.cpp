#include "test_runner.hpp"
#include "../modules/593_e_signature/e_signature.hpp"

namespace {

testing::TestOutcome run_e_signature() {
    e_signature::init();
    auto s = e_signature::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    e_signature::shutdown();
    return testing::ok();
}

const int _reg_e_signature = testing::register_test(
    "e_signature",
    "593_e_signature: stub status check",
    &run_e_signature);

}
