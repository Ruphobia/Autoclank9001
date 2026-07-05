#include "test_runner.hpp"
#include "../modules/366_osint_aggregator/osint_aggregator.hpp"

namespace {

testing::TestOutcome run_osint_aggregator() {
    osint_aggregator::init();
    auto s = osint_aggregator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    osint_aggregator::shutdown();
    return testing::ok();
}

const int _reg_osint_aggregator = testing::register_test(
    "osint_aggregator",
    "366_osint_aggregator: stub status check",
    &run_osint_aggregator);

}
