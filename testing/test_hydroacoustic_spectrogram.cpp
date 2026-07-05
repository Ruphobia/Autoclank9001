#include "test_runner.hpp"
#include "../modules/1550_hydroacoustic_spectrogram/hydroacoustic_spectrogram.hpp"

namespace {

testing::TestOutcome run_hydroacoustic_spectrogram() {
    hydroacoustic_spectrogram::init();
    auto s = hydroacoustic_spectrogram::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hydroacoustic_spectrogram::shutdown();
    return testing::ok();
}

const int _reg_hydroacoustic_spectrogram = testing::register_test(
    "hydroacoustic_spectrogram",
    "1550_hydroacoustic_spectrogram: stub status check",
    &run_hydroacoustic_spectrogram);

}
