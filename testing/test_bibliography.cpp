// Smoke test for 062_bibliography.

#include "test_runner.hpp"
#include "../modules/062_bibliography/bibliography.hpp"

namespace {

testing::TestOutcome run() {
    bibliography::init();

    nlohmann::json a = {
        {"id","bauer2026capybara"},
        {"author", nlohmann::json::array({
            {{"family","Bauer"},{"given","Jane"}},
            {{"family","Lee"},  {"given","Sam"}}
        })},
        {"issued", {{"date-parts", nlohmann::json::array({{2026}})}}},
        {"title", "Capybara thermoregulation"},
        {"container-title", "Journal of Mammalogy"},
        {"volume", "17"}, {"issue", "3"}, {"page", "201-215"},
        {"DOI", "10.1234/jmamm.2026"}
    };
    nlohmann::json b = {
        {"id","ramsay1904argon"},
        {"author", nlohmann::json::array({{{"family","Ramsay"},{"given","William"}}})},
        {"issued", {{"date-parts", nlohmann::json::array({{1904}})}}},
        {"title", "Argon and the noble gases"},
        {"publisher", "Macmillan"}
    };

    using bibliography::Style;
    using bibliography::Output;

    auto apa = bibliography::format_item(a, Style::ApaLike, Output::Plain);
    if (apa.find("Bauer, J., & Lee, S.") == std::string::npos)
        return testing::fail("APA author list wrong: " + apa);
    if (apa.find("(2026)") == std::string::npos)
        return testing::fail("APA year missing");
    if (apa.find("Journal of Mammalogy") == std::string::npos)
        return testing::fail("APA container missing");
    if (apa.find("17(3)") == std::string::npos || apa.find("201-215") == std::string::npos)
        return testing::fail("APA vol/issue/page missing");
    if (apa.find("doi.org/10.1234") == std::string::npos)
        return testing::fail("APA DOI link missing");

    auto ieee = bibliography::format_item(b, Style::IeeeNumeric, Output::Plain, 7);
    if (ieee.find("[7] W. Ramsay, \"Argon") == std::string::npos)
        return testing::fail("IEEE numeric wrong: " + ieee);

    auto chi = bibliography::format_item(a, Style::ChicagoNotes, Output::Markdown);
    if (chi.find("Bauer, Jane, and Sam Lee") == std::string::npos)
        return testing::fail("Chicago author order wrong: " + chi);
    if (chi.find("*Capybara thermoregulation*") == std::string::npos)
        return testing::fail("Chicago italics missing");

    auto html = bibliography::format_item(a, Style::ApaLike, Output::Html);
    if (html.find("<i>Journal of Mammalogy</i>") == std::string::npos)
        return testing::fail("APA HTML italics missing");

    // Full bibliography sorts APA by first-author family.
    auto bib = bibliography::format_bibliography({a, b}, Style::ApaLike);
    // Bauer < Ramsay alphabetically, so a should appear first.
    auto pa = bib.find("Bauer, J.");
    auto pb = bib.find("Ramsay, W.");
    if (pa == std::string::npos || pb == std::string::npos || pa >= pb)
        return testing::fail("APA sort order wrong");

    // Numeric bibliography preserves input order.
    auto bib_num = bibliography::format_bibliography({a, b}, Style::IeeeNumeric);
    if (bib_num.find("[1]") == std::string::npos ||
        bib_num.find("[2]") == std::string::npos)
        return testing::fail("Numeric bibliography missing indices");

    bibliography::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "bibliography",
    "062_bibliography: APA-ish + IEEE-numeric + Chicago-notes rendering",
    &run);

}
