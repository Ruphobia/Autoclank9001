#include "test_runner.hpp"
#include "../modules/141_whois_and_rdap_lookup/whois_and_rdap_lookup.hpp"

namespace {

testing::TestOutcome run_whois_and_rdap_lookup() {
    whois_and_rdap_lookup::init();
    auto s = whois_and_rdap_lookup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    whois_and_rdap_lookup::shutdown();
    return testing::ok();
}

const int _reg_whois_and_rdap_lookup = testing::register_test(
    "whois_and_rdap_lookup",
    "141_whois_and_rdap_lookup: stub status check",
    &run_whois_and_rdap_lookup);

}
