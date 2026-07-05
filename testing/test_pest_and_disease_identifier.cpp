#include "test_runner.hpp"
#include "../modules/1610_pest_and_disease_identifier/pest_and_disease_identifier.hpp"

namespace {

testing::TestOutcome run_pest_and_disease_identifier() {
    pest_and_disease_identifier::init();
    auto s = pest_and_disease_identifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pest_and_disease_identifier::shutdown();
    return testing::ok();
}

const int _reg_pest_and_disease_identifier = testing::register_test(
    "pest_and_disease_identifier",
    "1610_pest_and_disease_identifier: stub status check",
    &run_pest_and_disease_identifier);

}
