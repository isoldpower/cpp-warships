#include <application/core/errors/ErrorLayer.h>

#include <map>

namespace cpp_warships::core::errors {
    std::string_view nameOf(const ErrorLayer layer) noexcept {
        static const std::map<ErrorLayer, std::string_view> NAMES = {
            {ErrorLayer::Core, "core"},
            {ErrorLayer::Flow, "flow"},
            {ErrorLayer::Persistence, "persistence"},
            {ErrorLayer::Model, "model"},
            {ErrorLayer::Presentation, "presentation"},
        };

        const auto name = NAMES.find(layer);
        return name == NAMES.end() ? "unknown" : name->second;
    }
}  // namespace cpp_warships::core::errors
