#include "test_runner.hpp"
#include "../modules/351_pcap_analyzer/pcap_analyzer.hpp"

namespace {

testing::TestOutcome run_pcap_analyzer() {
    pcap_analyzer::init();
    auto s = pcap_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pcap_analyzer::shutdown();
    return testing::ok();
}

const int _reg_pcap_analyzer = testing::register_test(
    "pcap_analyzer",
    "351_pcap_analyzer: stub status check",
    &run_pcap_analyzer);

}
