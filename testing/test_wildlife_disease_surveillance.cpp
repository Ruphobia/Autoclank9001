#include "test_runner.hpp"
#include "../modules/1516_wildlife_disease_surveillance/wildlife_disease_surveillance.hpp"

namespace {

testing::TestOutcome run_wildlife_disease_surveillance() {
    wildlife_disease_surveillance::init();
    auto s = wildlife_disease_surveillance::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wildlife_disease_surveillance::shutdown();
    return testing::ok();
}

const int _reg_wildlife_disease_surveillance = testing::register_test(
    "wildlife_disease_surveillance",
    "1516_wildlife_disease_surveillance: stub status check",
    &run_wildlife_disease_surveillance);

}
