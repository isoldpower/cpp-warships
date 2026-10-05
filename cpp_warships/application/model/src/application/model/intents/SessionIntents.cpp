#include <application/model/ApplicationContext.h>
#include <application/model/behaviors/SaveBehavior.h>
#include <application/model/intents/SessionIntents.h>

#include <map>
#include <string>
#include <utility>

namespace cpp_warships::model::intents {
    SaveMatchIntent::SaveMatchIntent(behaviors::SaveBehavior& saves, std::string name) noexcept
        : saves_(saves)
        , name_(std::move(name)) {}

    std::string SaveMatchIntent::name() const {
        return "saving the match";
    }

    IntentResult SaveMatchIntent::apply() const {
        static const std::map<behaviors::SaveOutcome, std::string> FAILURES = {
            {behaviors::SaveOutcome::NoMatchInPlay, "there is no match to save"},
            {behaviors::SaveOutcome::CouldNotWrite, "the save could not be written"},
        };

        const behaviors::SaveOutcome outcome = saves_.saveMatch(name_);
        if (outcome == behaviors::SaveOutcome::Saved) {
            return IntentResult::succeeded();
        }

        const auto failure = FAILURES.find(outcome);
        return IntentResult::failed(
            failure == FAILURES.end() ? "the match could not be saved" : failure->second
        );
    }

    LoadMatchIntent::LoadMatchIntent(behaviors::SaveBehavior& saves, std::string slot) noexcept
        : saves_(saves)
        , slot_(std::move(slot)) {}

    std::string LoadMatchIntent::name() const {
        return "loading the match";
    }

    IntentResult LoadMatchIntent::apply() const {
        if (!saves_.loadMatch(slot_)) {
            return IntentResult::failed("that save could not be read");
        }

        return IntentResult::succeeded();
    }

    DeleteSaveIntent::DeleteSaveIntent(behaviors::SaveBehavior& saves, std::string slot) noexcept
        : saves_(saves)
        , slot_(std::move(slot)) {}

    std::string DeleteSaveIntent::name() const {
        return "deleting a save";
    }

    IntentResult DeleteSaveIntent::apply() const {
        if (!saves_.deleteSave(slot_)) {
            return IntentResult::failed("that save was already gone");
        }

        return IntentResult::succeeded();
    }

    FinishSessionIntent::FinishSessionIntent(ApplicationContext& application) noexcept
        : application_(application) {}

    std::string FinishSessionIntent::name() const {
        return "finishing the session";
    }

    IntentResult FinishSessionIntent::apply() const {
        application_.finish();
        return IntentResult::succeeded();
    }
}  // namespace cpp_warships::model::intents
