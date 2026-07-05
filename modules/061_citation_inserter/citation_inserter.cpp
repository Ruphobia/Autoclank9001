#include "citation_inserter.hpp"

#include <sstream>

namespace citation_inserter {
namespace {

std::string author_family(const nlohmann::json & item) {
    if (!item.contains("author") || !item["author"].is_array() ||
        item["author"].empty()) return "Anon";
    const auto & a = item["author"][0];
    if (!a.is_object()) return "Anon";
    std::string fam = a.value("family", "");
    if (fam.empty()) fam = a.value("literal", "");
    return fam.empty() ? std::string{"Anon"} : fam;
}

std::string year_full(const nlohmann::json & item) {
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

}

const char * to_string(Style s) {
    switch (s) {
        case Style::AuthorDate:        return "author-date";
        case Style::AuthorDateLocator: return "author-date-locator";
        case Style::Numeric:           return "numeric";
        case Style::Footnote:          return "footnote";
        case Style::NatbibTextual:     return "natbib-textual";
    }
    return "author-date";
}
const char * to_string(Format f) {
    switch (f) {
        case Format::Markdown: return "markdown";
        case Format::Pandoc:   return "pandoc";
        case Format::LaTeX:    return "latex";
        case Format::Typst:    return "typst";
        case Format::Org:      return "org";
        case Format::Plain:    return "plain";
    }
    return "plain";
}

void init()     {}
void shutdown() {}

std::string render_key(std::string_view key, std::string_view fam, std::string_view yr,
                       Style style, Format format,
                       std::string_view locator,
                       std::string_view prefix, std::string_view suffix) {
    std::ostringstream os;
    if (!prefix.empty()) os << prefix << " ";

    auto pandoc_inner = [&]() {
        std::ostringstream is;
        is << "@" << key;
        if (!locator.empty()) is << ", " << locator;
        return is.str();
    };

    switch (format) {
        case Format::Pandoc:
            switch (style) {
                case Style::AuthorDate:
                case Style::AuthorDateLocator:
                case Style::Footnote:
                    os << "[" << pandoc_inner() << "]";
                    break;
                case Style::Numeric:
                    os << "[" << pandoc_inner() << "]";
                    break;
                case Style::NatbibTextual:
                    os << "@" << key;
                    break;
            }
            break;
        case Format::LaTeX: {
            std::string cmd = "cite";
            if (style == Style::NatbibTextual) cmd = "citet";
            os << "\\" << cmd;
            if (!locator.empty()) os << "[" << locator << "]";
            os << "{" << key << "}";
            break;
        }
        case Format::Typst:
            os << "@" << key;
            if (!locator.empty()) os << "[" << locator << "]";
            break;
        case Format::Org:
            os << "[cite:@" << key;
            if (!locator.empty()) os << " " << locator;
            os << "]";
            break;
        case Format::Markdown:
        case Format::Plain:
        default:
            switch (style) {
                case Style::AuthorDate:        os << "(" << fam << ", " << yr << ")"; break;
                case Style::AuthorDateLocator:
                    os << "(" << fam << ", " << yr;
                    if (!locator.empty()) os << ", " << locator;
                    os << ")";
                    break;
                case Style::Numeric:           os << "[" << key << "]"; break;
                case Style::Footnote:          os << "^[" << fam << " " << yr << "]"; break;
                case Style::NatbibTextual:     os << fam << " (" << yr << ")"; break;
            }
            break;
    }
    if (!suffix.empty()) os << " " << suffix;
    return os.str();
}

std::string render(const nlohmann::json & item, Style style, Format format,
                   std::string_view locator,
                   std::string_view prefix, std::string_view suffix) {
    std::string key = item.value("id", std::string{"key"});
    std::string fam = author_family(item);
    std::string yr  = year_full(item);
    return render_key(key, fam, yr, style, format, locator, prefix, suffix);
}

}
