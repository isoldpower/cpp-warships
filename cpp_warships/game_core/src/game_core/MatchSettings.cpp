#include <game_core/MatchSettings.h>

#include <utility>

namespace cpp_warships::game_core {

    MatchSettings::MatchSettings(
            int boardSize,
            FleetComposition fleet,
            int baseDamage,
            int segmentHealth
    )
        : boardSize_(boardSize)
        , fleet_(std::move(fleet))
        , baseDamage_(baseDamage)
        , segmentHealth_(segmentHealth) {}

    MatchSettings MatchSettings::forBoardSize(int boardSize) {
        return MatchSettings{
            boardSize,
            FleetComposition::forBoardSize(boardSize)
        };
    }

    int MatchSettings::boardSize() const noexcept {
        return boardSize_;
    }

    const FleetComposition& MatchSettings::fleet() const noexcept {
        return fleet_;
    }

    int MatchSettings::baseDamage() const noexcept {
        return baseDamage_;
    }

    int MatchSettings::segmentHealth() const noexcept {
        return segmentHealth_;
    }
} // namespace cpp_warships::game_core
