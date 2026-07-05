#include "test_runner.hpp"
#include "../modules/1412_drug_identifier/drug_identifier.hpp"

namespace {

testing::TestOutcome run_drug_identifier() {
    drug_identifier::init();
    auto s = drug_identifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    drug_identifier::shutdown();
    return testing::ok();
}

const int _reg_drug_identifier = testing::register_test(
    "drug_identifier",
    "1412_drug_identifier: stub status check",
    &run_drug_identifier);

}
