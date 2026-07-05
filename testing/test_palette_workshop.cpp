#include "test_runner.hpp"
#include "../modules/299_palette_workshop/palette_workshop.hpp"

namespace {

testing::TestOutcome run_palette_workshop() {
    palette_workshop::init();
    auto s = palette_workshop::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    palette_workshop::shutdown();
    return testing::ok();
}

const int _reg_palette_workshop = testing::register_test(
    "palette_workshop",
    "299_palette_workshop: stub status check",
    &run_palette_workshop);

}
