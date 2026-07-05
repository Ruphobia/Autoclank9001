#include "test_runner.hpp"
#include "../modules/153_asn_and_bgp_browser/asn_and_bgp_browser.hpp"

namespace {

testing::TestOutcome run_asn_and_bgp_browser() {
    asn_and_bgp_browser::init();
    auto s = asn_and_bgp_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    asn_and_bgp_browser::shutdown();
    return testing::ok();
}

const int _reg_asn_and_bgp_browser = testing::register_test(
    "asn_and_bgp_browser",
    "153_asn_and_bgp_browser: stub status check",
    &run_asn_and_bgp_browser);

}
