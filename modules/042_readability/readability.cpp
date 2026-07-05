#include "readability.hpp"

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>

namespace readability {
namespace {

std::string strip_block(std::string s, const std::string & tag) {
    std::string open  = "<" + tag;
    std::string close = "</" + tag + ">";
    while (true) {
        auto a = s.find(open);
        if (a == std::string::npos) break;
        auto b = s.find(close, a);
        if (b == std::string::npos) {
            s.erase(a);
            break;
        }
        s.erase(a, b + close.size() - a);
    }
    return s;
}

std::string strip_html_tags(std::string s) {
    std::string out;
    out.reserve(s.size());
    bool in_tag = false;
    for (char c : s) {
        if (c == '<') { in_tag = true; continue; }
        if (c == '>') { in_tag = false; out.push_back(' '); continue; }
        if (!in_tag) out.push_back(c);
    }
    // Collapse whitespace.
    std::string collapsed;
    collapsed.reserve(out.size());
    bool ws = false;
    for (char c : out) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!ws) { collapsed.push_back(' '); ws = true; }
        } else { collapsed.push_back(c); ws = false; }
    }
    // Trim.
    size_t a = 0, b = collapsed.size();
    while (a < b && std::isspace(static_cast<unsigned char>(collapsed[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(collapsed[b - 1]))) --b;
    return collapsed.substr(a, b - a);
}

std::string find_meta(const std::string & html, const std::string & key_attr,
                      const std::string & key_value) {
    // Try the attribute order both ways.
    std::string re_str_1 = "<meta[^>]*" + key_attr + "=\"" + key_value +
                           "\"[^>]*content=\"([^\"]*)\"[^>]*>";
    std::string re_str_2 = "<meta[^>]*content=\"([^\"]*)\"[^>]*" + key_attr + "=\"" +
                           key_value + "\"[^>]*>";
    try {
        std::regex r1(re_str_1, std::regex::icase);
        std::smatch m;
        if (std::regex_search(html, m, r1)) return m[1].str();
        std::regex r2(re_str_2, std::regex::icase);
        if (std::regex_search(html, m, r2)) return m[1].str();
    } catch (...) {}
    return {};
}

std::string find_title(const std::string & html) {
    static const std::regex re("<title[^>]*>(.*?)</title>",
                               std::regex::icase | std::regex::ECMAScript);
    std::smatch m;
    if (std::regex_search(html, m, re)) return strip_html_tags(m[1].str());
    return {};
}

// Pick the highest-text-density region: prefer <article>, otherwise the
// <main> block, otherwise the longest <div>...</div>.
std::string pick_main_region(const std::string & html) {
    auto extract_first = [&](const std::string & tag) -> std::string {
        std::string open_attr = "<" + tag;
        std::string close     = "</" + tag + ">";
        auto a = html.find(open_attr);
        if (a == std::string::npos) return {};
        auto gt = html.find('>', a);
        if (gt == std::string::npos) return {};
        auto b = html.find(close, gt);
        if (b == std::string::npos) return {};
        return html.substr(gt + 1, b - (gt + 1));
    };

    std::string r = extract_first("article");
    if (!r.empty()) return r;
    r = extract_first("main");
    if (!r.empty()) return r;

    // Fallback: longest <div>...</div> by text length after stripping tags.
    std::string best;
    size_t best_len = 0;
    const std::string open_attr = "<div";
    const std::string close     = "</div>";
    size_t pos = 0;
    while (pos < html.size()) {
        auto a = html.find(open_attr, pos);
        if (a == std::string::npos) break;
        auto gt = html.find('>', a);
        if (gt == std::string::npos) break;
        auto b = html.find(close, gt);
        if (b == std::string::npos) break;
        std::string blk = html.substr(gt + 1, b - (gt + 1));
        std::string txt = strip_html_tags(blk);
        if (txt.size() > best_len) { best_len = txt.size(); best = blk; }
        pos = b + close.size();
    }
    return best;
}

}

void init()     {}
void shutdown() {}

Article extract(std::string_view html_v) {
    std::string html{html_v};

    // Boilerplate strip on a working copy of the HTML.
    std::string h = html;
    for (const auto & tag : {"script","style","noscript","header","footer","nav","aside","form"}) {
        h = strip_block(std::move(h), tag);
    }

    Article a;
    a.title        = find_meta(html, "property", "og:title");
    if (a.title.empty()) a.title = find_title(html);
    a.byline       = find_meta(html, "name", "author");
    if (a.byline.empty()) a.byline = find_meta(html, "property", "article:author");
    a.publish_date = find_meta(html, "property", "article:published_time");
    if (a.publish_date.empty()) a.publish_date = find_meta(html, "name", "date");
    if (a.publish_date.empty()) a.publish_date = find_meta(html, "property", "og:updated_time");

    std::string region = pick_main_region(h);
    if (region.empty()) region = h;
    a.body_text = strip_html_tags(region);
    return a;
}

}
