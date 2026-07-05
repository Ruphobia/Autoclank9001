#include "structured_data.hpp"

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>

namespace structured_data {
namespace {

std::string trim(std::string_view in) {
    size_t a = 0, b = in.size();
    while (a < b && std::isspace(static_cast<unsigned char>(in[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(in[b - 1]))) --b;
    return std::string(in.substr(a, b - a));
}

std::vector<std::string> json_ld_blocks(const std::string & html) {
    std::vector<std::string> out;
    // Tolerant: type may be "application/ld+json" with any attribute order.
    static const std::regex re(
        R"(<script[^>]*type=\"application/ld\+json\"[^>]*>([\s\S]*?)</script>)",
        std::regex::icase);
    auto begin = std::sregex_iterator(html.begin(), html.end(), re);
    for (auto it = begin; it != std::sregex_iterator(); ++it) {
        out.push_back((*it)[1].str());
    }
    return out;
}

std::map<std::string, std::string> opengraph_tags(const std::string & html) {
    std::map<std::string, std::string> out;
    // <meta property="og:title" content="..."/> and the other half of the
    // attribute-order universe.
    static const std::regex re_a(
        R"(<meta[^>]*property=\"((?:og|article|twitter):[^\"]+)\"[^>]*content=\"([^\"]*)\"[^>]*>)",
        std::regex::icase);
    static const std::regex re_b(
        R"(<meta[^>]*content=\"([^\"]*)\"[^>]*property=\"((?:og|article|twitter):[^\"]+)\"[^>]*>)",
        std::regex::icase);
    for (auto it = std::sregex_iterator(html.begin(), html.end(), re_a);
         it != std::sregex_iterator(); ++it) {
        out[(*it)[1].str()] = (*it)[2].str();
    }
    for (auto it = std::sregex_iterator(html.begin(), html.end(), re_b);
         it != std::sregex_iterator(); ++it) {
        out.emplace((*it)[2].str(), (*it)[1].str());
    }
    return out;
}

std::map<std::string, std::string> microdata_tags(const std::string & html) {
    std::map<std::string, std::string> out;
    // Two flavours: <tag itemprop="name" content="X"/> and inline text
    // <tag itemprop="name">X</tag>.
    static const std::regex re_content(
        R"(<[^>]*itemprop=\"([^\"]+)\"[^>]*content=\"([^\"]*)\")",
        std::regex::icase);
    for (auto it = std::sregex_iterator(html.begin(), html.end(), re_content);
         it != std::sregex_iterator(); ++it) {
        out.emplace((*it)[1].str(), (*it)[2].str());
    }
    // Loose closing tag: we accept any </name> rather than backref to the
    // opening, since std::regex backref + icase can misfire on some libstdc++
    // builds. False positives across mismatched tags are vanishingly rare
    // because the content is bounded to 200 non-'<' characters.
    static const std::regex re_text(
        R"(<[a-z][a-z0-9]*[^>]*itemprop=\"([^\"]+)\"[^>]*>([^<]{1,200})</[a-z][a-z0-9]*>)",
        std::regex::icase);
    for (auto it = std::sregex_iterator(html.begin(), html.end(), re_text);
         it != std::sregex_iterator(); ++it) {
        out.emplace((*it)[1].str(), trim((*it)[2].str()));
    }
    return out;
}

}

void init()     {}
void shutdown() {}

Extraction extract(std::string_view html_v) {
    std::string html{html_v};
    Extraction e;
    for (const auto & block : json_ld_blocks(html)) {
        try {
            auto j = nlohmann::json::parse(block);
            if (j.is_array()) {
                for (const auto & item : j) e.json_ld.push_back(item);
            } else {
                e.json_ld.push_back(j);
            }
        } catch (...) {
            // Skip malformed blocks; many sites publish broken JSON-LD.
        }
    }
    e.opengraph = opengraph_tags(html);
    e.microdata = microdata_tags(html);
    return e;
}

std::map<std::string, std::string> merge(const Extraction & e) {
    std::map<std::string, std::string> out;
    // JSON-LD first (top-level string-valued fields).
    for (const auto & doc : e.json_ld) {
        if (!doc.is_object()) continue;
        for (auto it = doc.begin(); it != doc.end(); ++it) {
            if (it.value().is_string()) {
                out.emplace(it.key(), it.value().get<std::string>());
            }
        }
    }
    // OpenGraph (key as-is; "og:title" -> "og:title").
    for (const auto & [k, v] : e.opengraph) out.emplace(k, v);
    // Microdata (raw itemprop names).
    for (const auto & [k, v] : e.microdata) out.emplace(k, v);
    return out;
}

}
