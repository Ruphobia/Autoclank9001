#include "test_runner.hpp"
#include "../modules/1681_photo_catalog/photo_catalog.hpp"

namespace {

testing::TestOutcome run_photo_catalog() {
    photo_catalog::init();
    auto s = photo_catalog::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    photo_catalog::shutdown();
    return testing::ok();
}

const int _reg_photo_catalog = testing::register_test(
    "photo_catalog",
    "1681_photo_catalog: stub status check",
    &run_photo_catalog);

}
