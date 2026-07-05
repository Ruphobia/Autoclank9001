#include "test_runner.hpp"
#include "../modules/194_ota_update_designer/ota_update_designer.hpp"

namespace {

testing::TestOutcome run_ota_update_designer() {
    ota_update_designer::init();
    auto s = ota_update_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ota_update_designer::shutdown();
    return testing::ok();
}

const int _reg_ota_update_designer = testing::register_test(
    "ota_update_designer",
    "194_ota_update_designer: stub status check",
    &run_ota_update_designer);

}
