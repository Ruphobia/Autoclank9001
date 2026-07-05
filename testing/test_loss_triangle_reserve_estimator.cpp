#include "test_runner.hpp"
#include "../modules/543_loss_triangle_reserve_estimator/loss_triangle_reserve_estimator.hpp"

namespace {

testing::TestOutcome run_loss_triangle_reserve_estimator() {
    loss_triangle_reserve_estimator::init();
    auto s = loss_triangle_reserve_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    loss_triangle_reserve_estimator::shutdown();
    return testing::ok();
}

const int _reg_loss_triangle_reserve_estimator = testing::register_test(
    "loss_triangle_reserve_estimator",
    "543_loss_triangle_reserve_estimator: stub status check",
    &run_loss_triangle_reserve_estimator);

}
