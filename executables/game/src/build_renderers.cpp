#include <application/head/common/PresentationContext.h>
#include <application/head/plain/PlainBattleView.h>
#include <application/head/plain/PlainMenuView.h>
#include <application/head/plain/PlainPlacementView.h>
#include <application/head/plain/PlainSaveBrowserView.h>
#include <application/head/plain/PlainSaveNamingView.h>
#include <application/head/tui/BattleView.h>
#include <application/head/tui/MenuView.h>
#include <application/head/tui/PlacementView.h>
#include <application/head/tui/SaveBrowserView.h>
#include <application/head/tui/SaveNamingView.h>
#include <build_renderers.h>

#include <memory>

namespace cpp_warships::application {
    namespace {
        /** @brief Drawn for an interactive terminal: colour, borders and a
         * mouse. */
        [[nodiscard]] head::common::render::RendererSet interactiveRenderers(
            head::common::PresentationContext& context
        ) {
            head::common::render::RendererSet renderers;

            renderers.drawScreenWith(
                head::common::ScreenKind::Menu,
                std::make_unique<head::tui::MenuView>(context, context.geometry())
            );
            renderers.drawScreenWith(
                head::common::ScreenKind::Placement,
                std::make_unique<head::tui::PlacementView>(context, context.geometry())
            );
            renderers.drawScreenWith(
                head::common::ScreenKind::Battle,
                std::make_unique<head::tui::BattleView>(context, context.geometry())
            );
            renderers.drawScreenWith(
                head::common::ScreenKind::Saves,
                std::make_unique<head::tui::SaveBrowserView>(context, context.geometry())
            );
            renderers.drawScreenWith(
                head::common::ScreenKind::SaveNaming,
                std::make_unique<head::tui::SaveNamingView>(context, context.geometry())
            );

            return renderers;
        }

        /** @brief Printed as plain text, the way the console game used to read. */
        [[nodiscard]] head::common::render::RendererSet plainRenderers(
            head::common::PresentationContext& context
        ) {
            head::common::render::RendererSet renderers;

            renderers.drawScreenWith(
                head::common::ScreenKind::Menu,
                std::make_unique<head::plain::PlainMenuView>(context)
            );
            renderers.drawScreenWith(
                head::common::ScreenKind::Placement,
                std::make_unique<head::plain::PlainPlacementView>(context)
            );
            renderers.drawScreenWith(
                head::common::ScreenKind::Battle,
                std::make_unique<head::plain::PlainBattleView>(context)
            );
            renderers.drawScreenWith(
                head::common::ScreenKind::Saves,
                std::make_unique<head::plain::PlainSaveBrowserView>(context)
            );
            renderers.drawScreenWith(
                head::common::ScreenKind::SaveNaming,
                std::make_unique<head::plain::PlainSaveNamingView>(context)
            );

            return renderers;
        }
    }  // namespace

    head::common::render::RendererSet buildRenderers(
        const ShellKind kind,
        head::common::PresentationContext& context
    ) {
        return kind == ShellKind::PlainTerminal ? plainRenderers(context)
                                                : interactiveRenderers(context);
    }
}  // namespace cpp_warships::application
