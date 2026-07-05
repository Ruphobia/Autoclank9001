#include "test_runner.hpp"
#include "../modules/1453_codebook_manager/codebook_manager.hpp"

namespace {

testing::TestOutcome run_codebook_manager() {
    codebook_manager::init();
    auto s = codebook_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    codebook_manager::shutdown();
    return testing::ok();
}

const int _reg_codebook_manager = testing::register_test(
    "codebook_manager",
    "1453_codebook_manager: stub status check",
    &run_codebook_manager);

}
