#include "test_runner.hpp"
#include "../modules/1536_paleogeographic_mapper/paleogeographic_mapper.hpp"

namespace {

testing::TestOutcome run_paleogeographic_mapper() {
    paleogeographic_mapper::init();
    auto s = paleogeographic_mapper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    paleogeographic_mapper::shutdown();
    return testing::ok();
}

const int _reg_paleogeographic_mapper = testing::register_test(
    "paleogeographic_mapper",
    "1536_paleogeographic_mapper: stub status check",
    &run_paleogeographic_mapper);

}
