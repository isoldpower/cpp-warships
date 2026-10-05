#include <application/head/common/PresentationContext.h>
#include <application/head/common/input/BattleInput.h>
#include <application/head/common/input/MenuInput.h>
#include <application/head/common/input/PlacementInput.h>
#include <application/head/common/input/SaveBrowserInput.h>
#include <application/head/common/input/SaveNamingInput.h>
#include <application/head/common/input/handlers/BattleHandlers.h>
#include <application/head/common/input/handlers/PlacementHandlers.h>
#include <application/head/common/input/handlers/SessionHandlers.h>
#include <application/model/intents/IntentFactory.h>
#include <application/model/scenarios/ScenarioQueue.h>
#include <build_event_pipeline.h>

namespace cpp_warships::application {
    std::unique_ptr<head::common::input::EventBus> buildEventBus(
        head::common::PresentationContext& context
    ) {
        auto bus = std::make_unique<head::common::input::EventBus>();

        bus->readScreenWith(
            head::common::ScreenKind::Menu,
            std::make_unique<head::common::input::MenuInput>(context)
        );
        bus->readScreenWith(
            head::common::ScreenKind::Placement,
            std::make_unique<head::common::input::PlacementInput>(context)
        );
        bus->readScreenWith(
            head::common::ScreenKind::Battle,
            std::make_unique<head::common::input::BattleInput>(context)
        );
        bus->readScreenWith(
            head::common::ScreenKind::Saves,
            std::make_unique<head::common::input::SaveBrowserInput>(context)
        );
        bus->readScreenWith(
            head::common::ScreenKind::SaveNaming,
            std::make_unique<head::common::input::SaveNamingInput>(context)
        );

        return bus;
    }

    namespace {
        namespace handlers = head::common::input::handlers;
        using model::events::EventScope;

        /** @brief What every handler of one session is built from. */
        struct SessionParts {
            handlers::HandlerParts parts;
            head::common::state::PresentationState& state;
            const SessionQueries& queries;
        };

        void subscribeSessionHandlers(
            model::events::EventRouter& router,
            const SessionParts& session
        ) {
            router.subscribe(
                EventScope::Always,
                std::make_shared<handlers::QuitHandler>(session.parts)
            );
            router.subscribe(
                EventScope::Always,
                std::make_shared<handlers::ReturnToMenuHandler>(session.state)
            );
            router.subscribe(
                EventScope::SaveNaming,
                std::make_shared<handlers::SaveAndQuitHandler>(session.parts, session.state)
            );
        }

        void subscribeMenuHandlers(
            model::events::EventRouter& router,
            const SessionParts& session
        ) {
            const SessionQueries& queries = session.queries;

            router.subscribe(
                EventScope::Menu,
                std::make_shared<handlers::StartMatchHandler>(session.parts, session.state)
            );
            router.subscribe(
                EventScope::Menu,
                std::make_shared<handlers::ResumeMatchHandler>(session.state, queries.hasMatch)
            );
            router.subscribe(
                EventScope::Menu,
                std::make_shared<handlers::OpenSaveNamingHandler>(
                    session.state,
                    queries.hasMatch,
                    queries.nameInPlay
                )
            );
            router.subscribe(
                EventScope::Menu,
                std::make_shared<handlers::OpenSaveBrowserHandler>(
                    session.state,
                    queries.hasMatch,
                    queries.hasSavedMatch
                )
            );
        }

        void subscribeSaveHandlers(
            model::events::EventRouter& router,
            const SessionParts& session
        ) {
            router.subscribe(
                EventScope::Saves,
                std::make_shared<handlers::LoadMatchHandler>(session.parts, session.state)
            );
            router.subscribe(
                EventScope::Saves,
                std::make_shared<handlers::DeleteSaveHandler>(session.parts, session.state)
            );
        }

        void subscribePlacementHandlers(
            model::events::EventRouter& router,
            const SessionParts& session
        ) {
            router.subscribe(
                EventScope::Placement,
                std::make_shared<handlers::PlaceShipHandler>(session.parts)
            );
            router.subscribe(
                EventScope::Placement,
                std::make_shared<handlers::RemoveShipHandler>(session.parts)
            );
            router.subscribe(
                EventScope::Placement,
                std::make_shared<handlers::ShuffleFleetHandler>(session.parts)
            );
            router.subscribe(
                EventScope::Placement,
                std::make_shared<handlers::BeginBattleHandler>(session.parts, session.queries.match)
            );
        }

        void subscribeBattleHandlers(
            model::events::EventRouter& router,
            const SessionParts& session
        ) {
            router.subscribe(
                EventScope::Battle,
                std::make_shared<handlers::FireHandler>(session.parts, session.queries.match)
            );
            router.subscribe(
                EventScope::Battle,
                std::make_shared<handlers::UseSkillHandler>(session.parts, session.queries.match)
            );
        }
    }  // namespace

    std::unique_ptr<model::events::EventRouter> buildEventRouter(
        const model::intents::IntentFactory& intents,
        model::scenarios::ScenarioQueue& scenarios,
        head::common::PresentationContext& context,
        const SessionQueries& queries
    ) {
        auto router = std::make_unique<model::events::EventRouter>();
        const SessionParts session{
            .parts = {.intents = intents, .scenarios = scenarios},
            .state = context.state(),
            .queries = queries
        };

        subscribeSessionHandlers(*router, session);
        subscribeMenuHandlers(*router, session);
        subscribeSaveHandlers(*router, session);
        subscribePlacementHandlers(*router, session);
        subscribeBattleHandlers(*router, session);

        return router;
    }
}  // namespace cpp_warships::application
