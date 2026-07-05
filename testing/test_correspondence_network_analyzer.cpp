#include "test_runner.hpp"
#include "../modules/697_correspondence_network_analyzer/correspondence_network_analyzer.hpp"

namespace {

testing::TestOutcome run_correspondence_network_analyzer() {
    correspondence_network_analyzer::init();
    auto s = correspondence_network_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    correspondence_network_analyzer::shutdown();
    return testing::ok();
}

const int _reg_correspondence_network_analyzer = testing::register_test(
    "correspondence_network_analyzer",
    "697_correspondence_network_analyzer: stub status check",
    &run_correspondence_network_analyzer);

}
