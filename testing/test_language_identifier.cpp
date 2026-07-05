#include "test_runner.hpp"
#include "../modules/794_language_identifier/language_identifier.hpp"

namespace {

testing::TestOutcome run_language_identifier() {
    language_identifier::init();
    auto s = language_identifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    language_identifier::shutdown();
    return testing::ok();
}

const int _reg_language_identifier = testing::register_test(
    "language_identifier",
    "794_language_identifier: stub status check",
    &run_language_identifier);

}
