#include "test_runner.hpp"
#include "../modules/617_comic_and_manga_reader/comic_and_manga_reader.hpp"

namespace {

testing::TestOutcome run_comic_and_manga_reader() {
    comic_and_manga_reader::init();
    auto s = comic_and_manga_reader::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    comic_and_manga_reader::shutdown();
    return testing::ok();
}

const int _reg_comic_and_manga_reader = testing::register_test(
    "comic_and_manga_reader",
    "617_comic_and_manga_reader: stub status check",
    &run_comic_and_manga_reader);

}
