#include "bibliography.hpp"

#include <algorithm>
#include <sstream>

namespace bibliography {
namespace {

std::string authors_apa(const nlohmann::json & item) {
    std::ostringstream os;
    if (!item.contains("author") || !item["author"].is_array()) return "Anon.";
    const auto & arr = item["author"];
    for (size_t i = 0; i < arr.size(); ++i) {
        if (!arr[i].is_object()) continue;
        std::string fam = arr[i].value("family", "");
        std::string giv = arr[i].value("given",  "");
        std::string initials;
        for (size_t j = 0, prev = 0; j <= giv.size(); ++j) {
            if (j == giv.size() || giv[j] == ' ' || giv[j] == '-') {
                if (j > prev) initials += std::string(1, giv[prev]) + ".";
                if (j < giv.size() && giv[j] == '-') initials += "-";
                prev = j + 1;
            }
        }
        if (i > 0) os << (i + 1 == arr.size() ? ", & " : ", ");
        os << fam;
        if (!initials.empty()) os << ", " << initials;
    }
    return os.str();
}

std::string authors_ieee(const nlohmann::json & item) {
    std::ostringstream os;
    if (!item.contains("author") || !item["author"].is_array()) return "Anon.";
    const auto & arr = item["author"];
    for (size_t i = 0; i < arr.size(); ++i) {
        if (!arr[i].is_object()) continue;
        std::string fam = arr[i].value("family", "");
        std::string giv = arr[i].value("given",  "");
        std::string init;
        if (!giv.empty()) init = giv.substr(0, 1) + ".";
        if (i > 0) os << ", ";
        if (!init.empty()) os << init << " ";
        os << fam;
    }
    return os.str();
}

std::string authors_chicago(const nlohmann::json & item) {
    // Bibliography form: Family, Given, and Given Family.
    std::ostringstream os;
    if (!item.contains("author") || !item["author"].is_array()) return "Anon.";
    const auto & arr = item["author"];
    for (size_t i = 0; i < arr.size(); ++i) {
        if (!arr[i].is_object()) continue;
        std::string fam = arr[i].value("family", "");
        std::string giv = arr[i].value("given",  "");
        if (i == 0) {
            os << fam;
            if (!giv.empty()) os << ", " << giv;
        } else {
            os << (i + 1 == arr.size() ? ", and " : ", ");
            if (!giv.empty()) os << giv << " ";
            os << fam;
        }
    }
    return os.str();
}

std::string year_str(const nlohmann::json & item) {
    if (!item.contains("issued")) return "n.d.";
    const auto & iss = item["issued"];
    if (iss.contains("date-parts") && iss["date-parts"].is_array() &&
        !iss["date-parts"].empty() && iss["date-parts"][0].is_array() &&
        !iss["date-parts"][0].empty()) {
        const auto & y = iss["date-parts"][0][0];
        if (y.is_number_integer()) return std::to_string(y.get<int>());
        if (y.is_string())         return y.get<std::string>();
    }
    if (iss.contains("literal") && iss["literal"].is_string())
        return iss["literal"].get<std::string>();
    return "n.d.";
}

std::string italicize(const std::string & s, Output out) {
    switch (out) {
        case Output::Markdown: return "*" + s + "*";
        case Output::Html:     return "<i>" + s + "</i>";
        default:               return s;
    }
}

std::string vol_issue_pages(const nlohmann::json & item) {
    std::ostringstream os;
    std::string vol = item.value("volume", "");
    std::string iss = item.value("issue",  "");
    std::string pg  = item.value("page",   "");
    if (!vol.empty()) os << vol;
    if (!iss.empty()) os << "(" << iss << ")";
    if (!pg.empty())  os << (vol.empty() && iss.empty() ? "" : ", ") << pg;
    return os.str();
}

}

const char * to_string(Style s) {
    switch (s) {
        case Style::ApaLike:      return "apa-like";
        case Style::IeeeNumeric:  return "ieee-numeric";
        case Style::ChicagoNotes: return "chicago-notes";
    }
    return "apa-like";
}
const char * to_string(Output o) {
    switch (o) {
        case Output::Plain:    return "plain";
        case Output::Markdown: return "markdown";
        case Output::Html:     return "html";
    }
    return "plain";
}

void init()     {}
void shutdown() {}

std::string format_item(const nlohmann::json & item, Style style, Output out,
                        int numeric_index) {
    std::ostringstream os;
    std::string title    = item.value("title",            "Untitled");
    std::string contain  = item.value("container-title",  "");
    std::string pub      = item.value("publisher",        "");

    switch (style) {
        case Style::ApaLike:
            os << authors_apa(item) << " (" << year_str(item) << "). "
               << title << ".";
            if (!contain.empty()) {
                os << " " << italicize(contain, out);
                std::string vip = vol_issue_pages(item);
                if (!vip.empty()) os << ", " << vip;
                os << ".";
            } else if (!pub.empty()) {
                os << " " << pub << ".";
            }
            if (item.contains("DOI") && !item["DOI"].get<std::string>().empty())
                os << " https://doi.org/" << item["DOI"].get<std::string>();
            break;
        case Style::IeeeNumeric:
            if (numeric_index > 0) os << "[" << numeric_index << "] ";
            os << authors_ieee(item) << ", \"" << title << ",\"";
            if (!contain.empty()) os << " " << italicize(contain, out) << ",";
            else if (!pub.empty()) os << " " << pub << ",";
            {
                std::string vip = vol_issue_pages(item);
                if (!vip.empty()) os << " " << vip << ",";
            }
            os << " " << year_str(item) << ".";
            break;
        case Style::ChicagoNotes:
            os << authors_chicago(item) << ". "
               << italicize(title, out) << ". ";
            if (!pub.empty()) os << pub << ", ";
            os << year_str(item) << ".";
            break;
    }
    return os.str();
}

std::string format_bibliography(const std::vector<nlohmann::json> & items,
                                Style style, Output out) {
    std::vector<nlohmann::json> sorted = items;
    if (style == Style::ApaLike || style == Style::ChicagoNotes) {
        std::sort(sorted.begin(), sorted.end(),
            [](const nlohmann::json & a, const nlohmann::json & b) {
                auto fam = [](const nlohmann::json & j) -> std::string {
                    if (!j.contains("author") || !j["author"].is_array() ||
                        j["author"].empty()) return std::string{};
                    return j["author"][0].value("family", "");
                };
                std::string fa = fam(a), fb = fam(b);
                if (fa != fb) return fa < fb;
                auto yr = [](const nlohmann::json & j) -> std::string {
                    if (!j.contains("issued")) return std::string{};
                    const auto & is = j["issued"];
                    if (is.contains("date-parts") && is["date-parts"].is_array() &&
                        !is["date-parts"].empty() && is["date-parts"][0].is_array() &&
                        !is["date-parts"][0].empty()) {
                        const auto & y = is["date-parts"][0][0];
                        if (y.is_number_integer()) return std::to_string(y.get<int>());
                        if (y.is_string())         return y.get<std::string>();
                    }
                    return std::string{};
                };
                return yr(a) < yr(b);
            });
    }
    std::ostringstream os;
    for (size_t i = 0; i < sorted.size(); ++i) {
        int idx = (style == Style::IeeeNumeric) ? (int)(i + 1) : 0;
        os << format_item(sorted[i], style, out, idx) << "\n";
    }
    return os.str();
}

}
