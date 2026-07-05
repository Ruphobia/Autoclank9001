#include "test_runner.hpp"
#include "../modules/1884_auction_price_comparator/auction_price_comparator.hpp"

namespace {

testing::TestOutcome run_auction_price_comparator() {
    auction_price_comparator::init();
    auto s = auction_price_comparator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    auction_price_comparator::shutdown();
    return testing::ok();
}

const int _reg_auction_price_comparator = testing::register_test(
    "auction_price_comparator",
    "1884_auction_price_comparator: stub status check",
    &run_auction_price_comparator);

}
