#include "test_runner.hpp"
#include "../modules/1484_autoguider/autoguider.hpp"

namespace {

testing::TestOutcome run_autoguider() {
    autoguider::init();
    auto s = autoguider::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    autoguider::shutdown();
    return testing::ok();
}

const int _reg_autoguider = testing::register_test(
    "autoguider",
    "1484_autoguider: stub status check",
    &run_autoguider);

}
