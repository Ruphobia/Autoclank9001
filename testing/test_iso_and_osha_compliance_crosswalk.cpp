#include "test_runner.hpp"
#include "../modules/475_iso_and_osha_compliance_crosswalk/iso_and_osha_compliance_crosswalk.hpp"

namespace {

testing::TestOutcome run_iso_and_osha_compliance_crosswalk() {
    iso_and_osha_compliance_crosswalk::init();
    auto s = iso_and_osha_compliance_crosswalk::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    iso_and_osha_compliance_crosswalk::shutdown();
    return testing::ok();
}

const int _reg_iso_and_osha_compliance_crosswalk = testing::register_test(
    "iso_and_osha_compliance_crosswalk",
    "475_iso_and_osha_compliance_crosswalk: stub status check",
    &run_iso_and_osha_compliance_crosswalk);

}
