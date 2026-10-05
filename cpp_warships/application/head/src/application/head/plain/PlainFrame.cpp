#include <application/head/common/render/Notices.h>
#include <application/head/plain/PlainFrame.h>

#include <cstddef>
#include <utility>

namespace cpp_warships::head::plain {
    namespace {
        constexpr std::size_t KEY_COLUMN_WIDTH = 8;
        constexpr const char* BANNER_RULE = "==============================";
    }  // namespace

    std::string plainKeyLine(const std::string& key, const std::string& description) {
        const std::size_t padding =
            key.size() < KEY_COLUMN_WIDTH ? KEY_COLUMN_WIDTH - key.size() : 1;
        return "  " + key + std::string(padding, ' ') + description;
    }

    std::vector<std::string> plainBanner(const std::string& centredTitle) {
        return {BANNER_RULE, centredTitle, BANNER_RULE};
    }

    PlainPage& PlainPage::line(std::string text) {
        lines_.push_back(std::move(text));
        return *this;
    }

    PlainPage& PlainPage::lines(const std::vector<std::string>& texts) {
        lines_.insert(lines_.end(), texts.begin(), texts.end());
        return *this;
    }

    PlainPage& PlainPage::blank() {
        return line("");
    }

    PlainPage& PlainPage::keys(const std::vector<PlainKey>& keys) {
        for (const auto& [key, description] : keys) {
            lines_.push_back(plainKeyLine(key, description));
        }
        return *this;
    }

    PlainPage& PlainPage::notices(const model::ApplicationContext& application) {
        for (const std::string& notice : common::render::noticesToShow(application)) {
            lines_.push_back("  ! " + notice);
        }
        return *this;
    }

    common::render::Frame PlainPage::frame() const {
        return common::render::frameOfLines(lines_);
    }
}  // namespace cpp_warships::head::plain
