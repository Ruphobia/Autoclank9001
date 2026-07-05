#include "test_runner.hpp"
#include "../modules/1662_watermark_and_provenance/watermark_and_provenance.hpp"

namespace {

testing::TestOutcome run_watermark_and_provenance() {
    watermark_and_provenance::init();
    auto s = watermark_and_provenance::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    watermark_and_provenance::shutdown();
    return testing::ok();
}

const int _reg_watermark_and_provenance = testing::register_test(
    "watermark_and_provenance",
    "1662_watermark_and_provenance: stub status check",
    &run_watermark_and_provenance);

}
