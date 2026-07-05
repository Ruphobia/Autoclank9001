#include "test_runner.hpp"
#include "../modules/1396_behavioral_assay_logger/behavioral_assay_logger.hpp"

namespace {

testing::TestOutcome run_behavioral_assay_logger() {
    behavioral_assay_logger::init();
    auto s = behavioral_assay_logger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    behavioral_assay_logger::shutdown();
    return testing::ok();
}

const int _reg_behavioral_assay_logger = testing::register_test(
    "behavioral_assay_logger",
    "1396_behavioral_assay_logger: stub status check",
    &run_behavioral_assay_logger);

}
