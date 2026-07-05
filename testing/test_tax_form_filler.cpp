#include "test_runner.hpp"
#include "../modules/558_tax_form_filler/tax_form_filler.hpp"

namespace {

testing::TestOutcome run_tax_form_filler() {
    tax_form_filler::init();
    auto s = tax_form_filler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tax_form_filler::shutdown();
    return testing::ok();
}

const int _reg_tax_form_filler = testing::register_test(
    "tax_form_filler",
    "558_tax_form_filler: stub status check",
    &run_tax_form_filler);

}
