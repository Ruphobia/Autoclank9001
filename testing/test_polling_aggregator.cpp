#include "test_runner.hpp"
#include "../modules/1452_polling_aggregator/polling_aggregator.hpp"

namespace {

testing::TestOutcome run_polling_aggregator() {
    polling_aggregator::init();
    auto s = polling_aggregator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    polling_aggregator::shutdown();
    return testing::ok();
}

const int _reg_polling_aggregator = testing::register_test(
    "polling_aggregator",
    "1452_polling_aggregator: stub status check",
    &run_polling_aggregator);

}
