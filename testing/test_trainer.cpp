#include "test_runner.hpp"
#include "../modules/1655_trainer/trainer.hpp"

namespace {

testing::TestOutcome run_trainer() {
    trainer::init();
    auto s = trainer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    trainer::shutdown();
    return testing::ok();
}

const int _reg_trainer = testing::register_test(
    "trainer",
    "1655_trainer: stub status check",
    &run_trainer);

}
