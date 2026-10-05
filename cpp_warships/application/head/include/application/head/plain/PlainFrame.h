#pragma once

#include <application/head/common/render/Frame.h>

#include <string>
#include <utility>
#include <vector>

namespace cpp_warships::model {
    class ApplicationContext;
}

namespace cpp_warships::head::plain {
    /** @brief One key of a legend: the key, and what pressing it does. */
    using PlainKey = std::pair<std::string, std::string>;

    /** @brief One entry of a key list, the key padded so the descriptions line up. */
    [[nodiscard]] std::string plainKeyLine(const std::string& key, const std::string& description);

    /** @brief A title framed by rules above and below, as the dialogs open with. */
    [[nodiscard]] std::vector<std::string> plainBanner(const std::string& centredTitle);

    /** @brief A plain screen put together line by line, top to bottom. */
    class PlainPage {
    public:
        PlainPage& line(std::string text);
        PlainPage& lines(const std::vector<std::string>& texts);
        PlainPage& blank();

        /** @brief Lists @p keys, one to a line, descriptions aligned. */
        PlainPage& keys(const std::vector<PlainKey>& keys);

        /** @brief Lists whatever the game has to say to the player just now. */
        PlainPage& notices(const model::ApplicationContext& application);

        [[nodiscard]] common::render::Frame frame() const;

    private:
        std::vector<std::string> lines_;
    };
}  // namespace cpp_warships::head::plain
