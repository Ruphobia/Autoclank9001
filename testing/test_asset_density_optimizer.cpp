#include "test_runner.hpp"
#include "../modules/162_asset_density_optimizer/asset_density_optimizer.hpp"

namespace {

testing::TestOutcome run_asset_density_optimizer() {
    asset_density_optimizer::init();
    auto s = asset_density_optimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    asset_density_optimizer::shutdown();
    return testing::ok();
}

const int _reg_asset_density_optimizer = testing::register_test(
    "asset_density_optimizer",
    "162_asset_density_optimizer: stub status check",
    &run_asset_density_optimizer);

}
