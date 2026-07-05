#include "test_runner.hpp"
#include "../modules/1614_crop_yield_model/crop_yield_model.hpp"

namespace {

testing::TestOutcome run_crop_yield_model() {
    crop_yield_model::init();
    auto s = crop_yield_model::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    crop_yield_model::shutdown();
    return testing::ok();
}

const int _reg_crop_yield_model = testing::register_test(
    "crop_yield_model",
    "1614_crop_yield_model: stub status check",
    &run_crop_yield_model);

}
