// Smoke test for 056_ref_formats.

#include "test_runner.hpp"
#include "../modules/056_ref_formats/ref_formats.hpp"

namespace {

const char * kBib = R"BIB(
@article{bauer2026capybara,
  title = {Capybara thermoregulation in subtropical wetlands},
  author = {Bauer, Jane and Lee, Sam},
  journal = {Journal of Mammalogy},
  year = {2026},
  volume = {17},
  number = {3},
  pages = {201--215},
  doi = {10.1234/jmamm.2026.0001},
  abstract = {We measure capybara surface temperature across seasons.}
}

@book{ramsay1904argon,
  title = "Argon and the noble gases",
  author = "Ramsay, William",
  year = "1904"
}
)BIB";

const char * kRis = R"RIS(TY  - JOUR
TI  - Capybara thermoregulation in subtropical wetlands
AU  - Bauer, Jane
AU  - Lee, Sam
JO  - Journal of Mammalogy
PY  - 2026
VL  - 17
IS  - 3
SP  - 201
EP  - 215
DO  - 10.1234/jmamm.2026.0001
ER  -
TY  - BOOK
TI  - Argon and the noble gases
AU  - Ramsay, William
PY  - 1904/01/01
ER  -
)RIS";

testing::TestOutcome run() {
    ref_formats::init();

    // BibTeX -> CSL-JSON.
    auto bib = ref_formats::parse_bibtex(kBib);
    if (bib.size() != 2)
        return testing::fail("bibtex parse count != 2: " + std::to_string(bib.size()));
    if (bib[0].value("type", "") != "article-journal")
        return testing::fail("bibtex type map wrong");
    if (bib[0].value("id", "") != "bauer2026capybara")
        return testing::fail("citation key not captured");
    if (bib[0]["author"].size() != 2)
        return testing::fail("bibtex author split wrong");
    if (bib[0]["author"][1].value("family", "") != "Lee")
        return testing::fail("bibtex 'and' split wrong");
    if (bib[0]["issued"]["date-parts"][0][0].get<int>() != 2026)
        return testing::fail("bibtex year -> issued wrong");
    if (bib[0].value("DOI", "") != "10.1234/jmamm.2026.0001")
        return testing::fail("bibtex DOI lost");
    if (bib[1].value("type", "") != "book")
        return testing::fail("bibtex book type wrong");

    // CSL-JSON -> BibTeX round trip.
    std::string back = ref_formats::to_bibtex(bib[0]);
    if (back.find("@article{bauer2026capybara") == std::string::npos)
        return testing::fail("to_bibtex header wrong");
    if (back.find("author = {Bauer, Jane and Lee, Sam}") == std::string::npos)
        return testing::fail("to_bibtex author serialize wrong");

    // RIS -> CSL-JSON.
    auto ris = ref_formats::parse_ris(kRis);
    if (ris.size() != 2) return testing::fail("ris count != 2");
    if (ris[0].value("type", "") != "article-journal")
        return testing::fail("ris type map wrong");
    if (ris[0]["author"].size() != 2)
        return testing::fail("ris author count wrong");
    if (ris[0].value("page", "") != "201-215")
        return testing::fail("ris SP+EP join wrong: '" + ris[0].value("page","") + "'");
    if (ris[0]["issued"]["date-parts"][0][0].get<int>() != 2026)
        return testing::fail("ris year parse wrong");

    // CSL-JSON passthrough.
    auto pt = ref_formats::parse_csl_json(
        R"([{"type":"book","title":"X"},{"type":"book","title":"Y"}])");
    if (pt.size() != 2 || pt[1].value("title","") != "Y")
        return testing::fail("csl-json passthrough wrong");

    // Sniffer.
    if (ref_formats::sniff_format("@article{a, ...") != "bibtex")
        return testing::fail("sniff bibtex");
    if (ref_formats::sniff_format("[{\"type\":\"x\"}]") != "csl-json")
        return testing::fail("sniff csl-json");
    if (ref_formats::sniff_format("TY  - JOUR\nER  - ") != "ris")
        return testing::fail("sniff ris");

    ref_formats::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "ref_formats",
    "056_ref_formats: BibTeX in/out + RIS in + CSL-JSON passthrough + sniffer",
    &run);

}
