#include "test_runner.hpp"
#include "../modules/607_adverse_media_scanner/adverse_media_scanner.hpp"

namespace {

testing::TestOutcome run_adverse_media_scanner() {
    adverse_media_scanner::init();
    auto s = adverse_media_scanner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    adverse_media_scanner::shutdown();
    return testing::ok();
}

const int _reg_adverse_media_scanner = testing::register_test(
    "adverse_media_scanner",
    "607_adverse_media_scanner: stub status check",
    &run_adverse_media_scanner);

}
