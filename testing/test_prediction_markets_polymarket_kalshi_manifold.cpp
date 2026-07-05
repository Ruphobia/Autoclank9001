#include "test_runner.hpp"
#include "../modules/552_prediction_markets_polymarket_kalshi_manifold/prediction_markets_polymarket_kalshi_manifold.hpp"

namespace {

testing::TestOutcome run_prediction_markets_polymarket_kalshi_manifold() {
    prediction_markets_polymarket_kalshi_manifold::init();
    auto s = prediction_markets_polymarket_kalshi_manifold::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    prediction_markets_polymarket_kalshi_manifold::shutdown();
    return testing::ok();
}

const int _reg_prediction_markets_polymarket_kalshi_manifold = testing::register_test(
    "prediction_markets_polymarket_kalshi_manifold",
    "552_prediction_markets_polymarket_kalshi_manifold: stub status check",
    &run_prediction_markets_polymarket_kalshi_manifold);

}
