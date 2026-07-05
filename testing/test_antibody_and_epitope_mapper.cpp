#include "test_runner.hpp"
#include "../modules/1365_antibody_and_epitope_mapper/antibody_and_epitope_mapper.hpp"

namespace {

testing::TestOutcome run_antibody_and_epitope_mapper() {
    antibody_and_epitope_mapper::init();
    auto s = antibody_and_epitope_mapper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    antibody_and_epitope_mapper::shutdown();
    return testing::ok();
}

const int _reg_antibody_and_epitope_mapper = testing::register_test(
    "antibody_and_epitope_mapper",
    "1365_antibody_and_epitope_mapper: stub status check",
    &run_antibody_and_epitope_mapper);

}
