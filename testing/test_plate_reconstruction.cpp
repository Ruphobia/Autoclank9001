#include "test_runner.hpp"
#include "../modules/1595_plate_reconstruction/plate_reconstruction.hpp"

namespace {

testing::TestOutcome run_plate_reconstruction() {
    plate_reconstruction::init();
    auto s = plate_reconstruction::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    plate_reconstruction::shutdown();
    return testing::ok();
}

const int _reg_plate_reconstruction = testing::register_test(
    "plate_reconstruction",
    "1595_plate_reconstruction: stub status check",
    &run_plate_reconstruction);

}
