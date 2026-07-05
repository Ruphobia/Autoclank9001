#include "test_runner.hpp"
#include "../modules/1404_icd_cpt_snomed_lookup/icd_cpt_snomed_lookup.hpp"

namespace {

testing::TestOutcome run_icd_cpt_snomed_lookup() {
    icd_cpt_snomed_lookup::init();
    auto s = icd_cpt_snomed_lookup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    icd_cpt_snomed_lookup::shutdown();
    return testing::ok();
}

const int _reg_icd_cpt_snomed_lookup = testing::register_test(
    "icd_cpt_snomed_lookup",
    "1404_icd_cpt_snomed_lookup: stub status check",
    &run_icd_cpt_snomed_lookup);

}
